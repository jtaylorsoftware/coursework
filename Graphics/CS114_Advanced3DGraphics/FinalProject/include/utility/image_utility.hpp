/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "stb_image.h"

#include "opengl/texture.hpp"
#include "opengl/texture2d.hpp"

// Remove Win32 LoadImage macro
#ifdef LoadImage
#undef LoadImage
#endif

struct Image
{
	Image();

	Image(Image&&) = default;
	Image& operator=(Image&&) = default;

	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;

	unsigned char* data;
	int width;
	int height;
	int numComponents;
};

// Utility static class that loads images using stb image library
class ImageLoader
{
public:
	static Image LoadImage(std::string filePath, bool flipVertically);
	static void FreeImage(Image& image);
private:
};
