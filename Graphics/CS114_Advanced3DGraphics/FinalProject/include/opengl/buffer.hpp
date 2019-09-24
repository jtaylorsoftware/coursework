/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include <glad/glad.h>

// Base class for any OpenGL buffer object (i.e, vertex buffer object)
class Buffer
{
public:
	Buffer();
	Buffer(Buffer&& buffer);
	~Buffer();

	Buffer& operator=(Buffer&& buffer);

	inline GLuint GetId() const;
protected:
	GLuint id;
private:
	static GLuint MoveId(Buffer& other);
	void Delete();
};

inline GLuint Buffer::GetId() const{
	return id;
}
