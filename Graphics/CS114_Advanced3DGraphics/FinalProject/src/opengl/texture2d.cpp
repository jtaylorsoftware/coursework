/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/


#include "opengl/texture2d.hpp"

Texture2d::Texture2d(){
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
}

Texture2d::~Texture2d(){
}

void Texture2d::AllocateStorage(TextureStorageFormat format){
	glTextureStorage2D(id, format.levels, format.internalFormat, format.width, format.height);
}

void Texture2d::SetImage(TextureImage image){
	glTextureSubImage2D(id, image.format.level, image.format.offsetX,
		image.format.offsetY, image.format.width, image.format.height,
		image.format.dataFormat, image.format.dataType, image.data);
}
