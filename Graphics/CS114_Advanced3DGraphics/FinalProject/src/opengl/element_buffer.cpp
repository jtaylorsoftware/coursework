/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "opengl/element_buffer.hpp"

ElementBuffer::ElementBuffer(){
}

ElementBuffer::ElementBuffer(const std::vector<unsigned>& indices){
	CreateBufferFromIndices(indices);
}

ElementBuffer::~ElementBuffer(){
}

void ElementBuffer::SetData(const std::vector<unsigned>& indices){
	glNamedBufferData(id, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
}

void ElementBuffer::CreateBufferFromIndices(const std::vector<unsigned>& indices){
	glNamedBufferData(id, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
}
