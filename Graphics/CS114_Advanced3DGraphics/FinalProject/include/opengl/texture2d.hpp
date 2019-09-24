/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "opengl/texture.hpp"

// Wraps OpenGL texture object functionality, extended to use TEXTURE_2D as the 
// texture target, and uses other Direct State Access functionality to update
// state without binding
class Texture2d : public Texture
{
public:
	Texture2d();
	Texture2d(Texture2d&& texture) = default;
	~Texture2d();

	Texture2d& operator=(Texture2d&& texture) = default;

	void AllocateStorage(TextureStorageFormat format);
	void SetImage(TextureImage image);

	inline GLenum GetTarget();
private:

};

inline GLenum Texture2d::GetTarget(){
	return GL_TEXTURE_2D;
}