/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "opengl/vertex_array.hpp"

VertexArray::VertexArray() : id(0){
	glCreateVertexArrays(1, &id);
}

VertexArray::VertexArray(VertexArray && vertexArray){
	id = vertexArray.id;
	vertexArray.id = 0;
}

VertexArray::~VertexArray(){
}

VertexArray & VertexArray::operator=(VertexArray && vertexArray){
	id = vertexArray.id;
	vertexArray.id = 0;
	return *this;
}

void VertexArray::SetIndexBuffer(const ElementBuffer & buffer){
	glVertexArrayElementBuffer(id, buffer.GetId());
}

void VertexArray::AddVertexBuffer(const VertexBuffer& buffer, const VertexBufferFormat& format){
	glVertexArrayVertexBuffer(id, format.bindingIndex, buffer.GetId(), format.offset, format.stride);
}

void VertexArray::AddAttribute(const VertexArrayAttribute & attribute){
	glVertexArrayAttribFormat(id, attribute.attributeIndex, attribute.format.size, attribute.format.type,
		attribute.format.normalized, attribute.format.relativeOffset);
	glVertexArrayAttribBinding(id, attribute.attributeIndex, attribute.vertexBindingIndex);
}

void VertexArray::EnableAttribute(GLuint attribIndex){
	glEnableVertexArrayAttrib(id, attribIndex);
}
