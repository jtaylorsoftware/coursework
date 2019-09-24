/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include <vector>

#include <glad/glad.h>
#include "utility/model_types.hpp"
#include "opengl/buffer.hpp"

struct VertexBufferFormat
{
	GLuint bindingIndex;
	GLintptr offset;
	GLuint stride;
};

// Wraps the OpenGL vertex buffer object functionality to make it simpler to understand the setup process
class VertexBuffer : public Buffer
{
public:
	VertexBuffer();
	VertexBuffer(const std::vector<Vertex>& vertices);
	VertexBuffer(VertexBuffer&& vertexBuffer) = default;
	~VertexBuffer();

	VertexBuffer& operator=(VertexBuffer&& vertexBuffer) = default;

	void SetData(const std::vector<Vertex>& vertices);

private:
	void CreateBufferFromVertices(const std::vector<Vertex>& vertices);
};

