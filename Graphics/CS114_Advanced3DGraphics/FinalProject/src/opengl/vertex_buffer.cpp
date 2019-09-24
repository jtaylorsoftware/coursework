/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "opengl/vertex_buffer.hpp"

VertexBuffer::VertexBuffer(){
}

VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices){
	CreateBufferFromVertices(vertices);
}

VertexBuffer::~VertexBuffer(){
}

void VertexBuffer::SetData(const std::vector<Vertex>& vertices){
	glNamedBufferData(id, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void VertexBuffer::CreateBufferFromVertices(const std::vector<Vertex>& vertices){
	glNamedBufferData(id, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}
