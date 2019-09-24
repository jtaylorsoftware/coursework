/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/


#include "opengl/texture.hpp"

Texture::Texture() : id(0){
}

Texture::Texture(Texture&& texture){
	id = texture.id;
	texture.id = 0;
}

Texture::~Texture(){
	Delete();
}

Texture & Texture::operator=(Texture&& texture){
	Delete();

	id = texture.id;
	texture.id = 0;

	return *this;
}

void Texture::SetParameter(GLenum paramName, GLfloat value){
	glTextureParameterf(id, paramName, value);
}

void Texture::SetParameter(GLenum paramName, GLint value){
	glTextureParameteri(id, paramName, value);
}

void Texture::Delete(){
	if (id != 0){
		glDeleteTextures(1, &id);
	}
}
