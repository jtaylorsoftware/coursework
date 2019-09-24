/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include <glad/glad.h>

#include "opengl/element_buffer.hpp"
#include "opengl/vertex_buffer.hpp"

struct AttributeFormat
{
	GLuint size;
	GLenum type;
	GLboolean normalized;
	GLuint relativeOffset;
};

struct VertexArrayAttribute
{
	AttributeFormat format;
	GLuint vertexBindingIndex;
	GLuint attributeIndex;
};

// Wraps OpenGL vertex array object functionality
class VertexArray
{
public:
	VertexArray();
	VertexArray(VertexArray&& vertexArray);
	~VertexArray();

	VertexArray& operator=(VertexArray&& vertexArray);

	void SetIndexBuffer(const ElementBuffer& buffer);

	void AddVertexBuffer(const VertexBuffer& buffer, const VertexBufferFormat& format);

	void AddAttribute(const VertexArrayAttribute& attribute);
	void EnableAttribute(GLuint attribIndex);

	inline GLuint GetId() const;
private:
	GLuint id;
};

inline GLuint VertexArray::GetId() const{
	return id;
}