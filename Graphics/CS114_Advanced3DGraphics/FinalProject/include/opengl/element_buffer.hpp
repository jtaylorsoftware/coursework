/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include <glad/glad.h>
#include <vector>

#include "opengl/buffer.hpp"

// Used to create element buffer objects for indexed drawing
class ElementBuffer : public Buffer
{
public:
	ElementBuffer();
	ElementBuffer(const std::vector<unsigned>& indices);
	ElementBuffer(ElementBuffer && elementBuffer) = default;
	~ElementBuffer();

	ElementBuffer& operator=(ElementBuffer&& elementBuffer) = default;

	void SetData(const std::vector<unsigned>& indices);

private:
	void CreateBufferFromIndices(const std::vector<unsigned>& indices);
};

