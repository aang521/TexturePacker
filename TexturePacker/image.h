#pragma once

struct Image;

void DestroyImage(Image* image);

void LoadImage(char * file, Image* image);

struct Image
{
	u32 width, height;
	u32 x, y;
	Color* pixels = NULL;
	char* file;
	void destroy() { DestroyImage(this); }
};
