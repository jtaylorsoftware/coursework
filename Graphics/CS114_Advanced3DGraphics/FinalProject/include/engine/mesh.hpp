/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/
#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "utility/model_types.hpp"
#include "engine/material.hpp"

#include "opengl/element_buffer.hpp"
#include "opengl/vertex_array.hpp"
#include "opengl/vertex_buffer.hpp"

// Encapsulates the data and logic for setting up the OpenGL objects (vao, vbo, ebo) 
// needed to draw a triangle mesh
class Mesh
{
public:
	Mesh();
	~Mesh();

	static Mesh Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

	Mesh(Mesh&& mesh);

	Mesh& operator=(Mesh&& mesh);

	GLuint GetVertexBuffer() const;
	GLuint GetElementBuffer() const;
	GLuint GetVertexArray() const;

	GLsizei GetIndicesCount() const;

	void Delete();

	Material material;
private:
	ElementBuffer elementBuffer;
	VertexBuffer vertexBuffer;
	VertexArray vertexArray;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
