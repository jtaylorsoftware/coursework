/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/


#include "opengl/buffer.hpp"

Buffer::Buffer() : id(0){
	glCreateBuffers(1, &id);
}

Buffer::Buffer(Buffer && buffer){
	id = Buffer::MoveId(buffer);
}

Buffer::~Buffer(){
	Delete();
}

Buffer & Buffer::operator=(Buffer && buffer){
	Delete();
	id = Buffer::MoveId(buffer);
	return *this;
}

GLuint Buffer::MoveId(Buffer & other){
	GLuint id = other.id;
	other.id = 0;
	return id;
}

void Buffer::Delete(){
	if (id != 0){
		glDeleteBuffers(1, &id);
	}
}
