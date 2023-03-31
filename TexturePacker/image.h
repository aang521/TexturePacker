#pragma once

struct Image;

void DestroyImage(Image* image);

void LoadImage(char * file, Image* image);

struct Image
{
	i32 width, height;
	i32 x, y;
	Color* pixels = NULL;
	char* file;
	void destroy() { DestroyImage(this); }
};
