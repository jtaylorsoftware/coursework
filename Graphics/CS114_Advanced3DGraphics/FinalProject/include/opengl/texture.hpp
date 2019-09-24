/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include <glad/glad.h>

struct TextureStorageFormat
{
	GLsizei levels;
	GLenum internalFormat;
	GLsizei width;
	GLsizei height;
};

struct TextureImageFormat
{
	GLint level;
	GLint offsetX;
	GLint offsetY;
	GLsizei width;
	GLsizei height;
	GLenum dataFormat;
	GLenum dataType;
};

struct TextureImage
{
	TextureImageFormat format;
	const void* data;
};

// Base class that wraps OpenGL texture objects, geared towards using Direct State Access
// to allow bindless state updates
class Texture
{
public:
	Texture();
	Texture(Texture&& texture);
	virtual ~Texture();

	Texture& operator=(Texture&& texture);

	virtual void AllocateStorage(TextureStorageFormat format) = 0;
	virtual void SetImage(TextureImage image) = 0;

	void SetParameter(GLenum paramName, GLfloat value);
	void SetParameter(GLenum paramName, GLint value);

	inline GLuint GetId();
	virtual inline GLenum GetTarget() = 0;
protected:
	GLuint id;
private:
	void Delete();
};

inline GLuint Texture::GetId(){
	return id;
}
