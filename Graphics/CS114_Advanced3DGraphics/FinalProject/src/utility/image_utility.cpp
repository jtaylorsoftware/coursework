/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#define STB_IMAGE_IMPLEMENTATION
#include "utility/image_utility.hpp"

Image::Image() : width(0), height(0), numComponents(0), data(nullptr){
}

Image ImageLoader::LoadImage(std::string filePath, bool flipVertically){
	Image image;
	stbi_set_flip_vertically_on_load((int)flipVertically);
	image.data = stbi_load(filePath.c_str(), &image.width, &image.height, &image.numComponents, 0);
	return image;
}

void ImageLoader::FreeImage(Image & image){
	stbi_image_free(image.data);
}
