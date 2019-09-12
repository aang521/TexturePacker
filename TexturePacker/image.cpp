#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#ifdef LoadImage
#undef LoadImage
#endif

void DestroyImage(Image * image)
{
	if (image->pixels)
		delete[] image->pixels;
	image->pixels = NULL;
	delete[] image->file;
}

void LoadImage(char * file, Image* image)
{
	image->file = new char[strlen(file) + 1];
	memcpy(image->file, file, strlen(file) + 1);
	int texChannels;
	byte* data = stbi_load(file, (int*)&image->width, (int*)&image->height, &texChannels, STBI_rgb_alpha);
	if (!data)
	{
		LogError("failed to load image! %s", file);
		image->pixels = new Color(0xff00ff);
		image->width = 1;
		image->height = 1;
		return;
	}

	image->pixels = new Color[image->width * image->height];
	memcpy(image->pixels, data, sizeof(Color) * image->width * image->height);

	stbi_image_free(data);
}
