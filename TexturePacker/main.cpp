#include "image.h"

#pragma warning(disable:4996)

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#include <Windows.h>
#include <vector>
#include <algorithm>

using namespace std;

#ifdef LoadImage
#undef LoadImage
#endif

#include <direct.h>

bool SortByHeightFunc(Image& a, Image& b)
{
	return a.height > b.height;
}

void CreatePackedTexture(char* outputFile, List<char*> &inputFiles)
{
	List<Image> sourceImages(inputFiles.length(), 0);
	for (int i = 0; i < inputFiles.length(); i++)
	{
		LoadImage(inputFiles[i], sourceImages.add());
	}

	sort(sourceImages.first(), sourceImages.last(), SortByHeightFunc);

	List<Image> smallest = sourceImages;

	int height = sourceImages[0].height;
	int widestFrame = sourceImages[0].width;
	int width = 0;
	int totalArea = 0;
	for (unsigned int i = 0; i < sourceImages.size(); i++)
	{
		width += sourceImages[i].width;
		widestFrame = MAX(widestFrame, sourceImages[i].width);
		totalArea += sourceImages[i].width * sourceImages[i].height;
	}

	int smallestArea = width * height;
	int smallestWidth = width;
	int smallestHeight = height;
	bool done = false;
	while (!done)
	{
		vector<vector<bool>> cells;
		List<int> widths;
		List<int> heights;

		cells.resize(1);
		cells[0].resize(1);
		cells[0][0] = true;
		widths.add(width);
		heights.add(height);
		bool placedAll = true;
		for (unsigned int i = 0; i < sourceImages.size(); i++)
		{
			bool found = false;
			for (unsigned int y = 0; y < cells.size() && !found; y++)
				for (unsigned int x = 0; x < cells[y].size() && !found; x++)
					if (cells[y][x])
					{
						int endx = x;
						int endy = y;
						int width = widths[x];
						int height = heights[y];
						bool fits = true;
						while (width < sourceImages[i].width)
						{
							endx++;
							if (endx >= widths.size())
							{
								fits = false;
								break;
							}
							width += widths[endx];
						}
						if (!fits)
							continue;
						while (height < sourceImages[i].height)
						{
							endy++;
							if (endy >= heights.size())
							{
								fits = false;
								break;
							}
							height += heights[endy];
						}
						if (!fits)
							continue;
						for (int ty = y; ty <= endy && fits; ty++)
							for (int tx = x; tx <= endx && fits; tx++)
								if (!cells[ty][tx])
									fits = false;
						if (!fits)
							continue;

						int cWidth = sourceImages[i].width;
						for (int tx = x; tx <= endx; tx++)
						{
							if (cWidth >= widths[tx])
								cWidth -= widths[tx];
							else
							{
								widths.insert(tx + 1, widths[tx] - cWidth);
								for (int j = 0; j < cells.size(); j++)
									cells[j].insert(cells[j].begin() + tx + 1, cells[j][tx]);
								widths[tx] = cWidth;
								break;
							}
						}

						int cHeight = sourceImages[i].height;
						for (int ty = y; ty <= endy; ty++)
						{
							if (cHeight >= heights[ty])
								cHeight -= heights[ty];
							else
							{
								heights.insert(ty + 1, heights[ty] - cHeight);
								cells.insert(cells.begin() + ty + 1, cells[ty]);
								heights[ty] = cHeight;
								break;
							}
						}

						for (int ty = y; ty <= endy; ty++)
							for (int tx = x; tx <= endx; tx++)
								cells[ty][tx] = false;

						int xpos = 0;
						for (int tx = 0; tx < x; tx++)
							xpos += widths[tx];
						int ypos = 0;
						for (int ty = 0; ty < y; ty++)
							ypos += heights[ty];
						sourceImages[i].x = xpos;
						sourceImages[i].y = ypos;

						found = true;
					}
			placedAll = found;
			if (!found)
				break;
		}
		if (placedAll)
		{
			if (width * height <= smallestArea)
			{
				smallestArea = width * height;
				smallest = sourceImages;
				smallestWidth = width;
				smallestHeight = height;
			}
			width--;
			if (width < widestFrame)
			{
				done = true;
				break;
			}
			bool found = false;
			while (!found)
			{
				while (width*height < totalArea)
					height++;
				if (width*height > smallestArea)
				{
					width--;
					if (width < widestFrame)
					{
						done = true;
						found = true;
						break;
					}
				}
				else
					found = true;
			}

		}
		else
		{
			height++;
			//change this to increase by percise amout for better performance
		}
	}

	width = smallestWidth;
	height = smallestHeight;

	Color* data = new Color[width*height];
	memset(data, 0, sizeof(Color)*width*height);
	for (int i = 0; i < smallest.size(); i++)
	{
		int srcWidth = smallest[i].width;
		int x = smallest[i].x;
		for (unsigned int y = 0; y < smallest[i].height; y++)
			memcpy(&data[x + (y + smallest[i].y)*width], &smallest[i].pixels[y*srcWidth], sizeof(Color)*srcWidth);
	}

	FILE* f = fopen("out.txt", "w");
	char buff[1024];
	for (int i = 0; i < smallest.length(); i++)
	{
		sprintf(buff, "%s: %i %i %i %i\n", smallest[i].file, smallest[i].x, smallest[i].y, smallest[i].width, smallest[i].height);
		fwrite(buff,1, strlen(buff), f);
	}
	fclose(f);

	if( stbi_write_png(outputFile, width, height, 4, data, 4 * width) == 0 )
	{
		LogError("Error writing image");
	}
}

void SelectInputFiles(List<char*> &inputFiles)
{
	char* dir = _getcwd(NULL, 0);

	OPENFILENAME ofn;
	char fileName[MAX_PATH*100] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "PNG\0*.png\0JPG\0*.jpg\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH*100;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;
	char temp[MAX_PATH];
	sprintf_s(temp, "%s\\out", _getcwd(NULL, 0));
	ofn.lpstrInitialDir = temp;
	ofn.lpstrDefExt = "";
	GetOpenFileName(&ofn);

	_chdir(dir);

	int error = GetLastError();

	char* files = fileName;
	while (int len = strlen(files))
	{
		char* buff = inputFiles.add((char*)malloc(len+1));
		memcpy(buff, files, len+1);
		files += len + 1;
	}

	if (inputFiles.length() > 1)
	{
		int pathLength = strlen(inputFiles[0]);
		char path[MAX_PATH];
		memcpy(path, inputFiles[0], pathLength+1);
		path[pathLength] = '\\';
		for (int i = 1; i < inputFiles.length(); i++)
		{
			int fileLen = strlen(inputFiles[i]);
			free(inputFiles[i - 1]);
			char* buff = inputFiles[i-1] = (char*)malloc(pathLength + fileLen + 2);
			memcpy(buff, path, pathLength+1);
			memcpy(buff + pathLength + 1, inputFiles[i], fileLen+1);
		}
		inputFiles.removeNoResize(inputFiles.length() - 1);
	}
}

int __cdecl main(int argc, char* argv[])
{
	List<char*> inputFiles;
	SelectInputFiles(inputFiles);
	for (int i = 0; i < inputFiles.length(); i++)
	{
		LogInfo("%s", inputFiles[i]);
	}

	char* dir = _getcwd(NULL, 0);

	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "PNG\0*.png\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	char temp[MAX_PATH];
	sprintf_s(temp, "%s\\out", _getcwd(NULL, 0));
	ofn.lpstrInitialDir = temp;
	ofn.lpstrDefExt = "";
	GetSaveFileName(&ofn);

	_chdir(dir);

	CreatePackedTexture(fileName, inputFiles);

	system("pause");
}