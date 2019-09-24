/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/mesh.hpp"

Mesh::Mesh(){
}

Mesh::~Mesh(){
	Delete();
}

Mesh Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices){
	Mesh mesh;
	mesh.vertices = vertices;
	mesh.indices = indices;

	mesh.vertexBuffer.SetData(vertices);
	mesh.elementBuffer.SetData(indices);

	constexpr GLuint vertexBinding = 0, positionIndex = 0, normalIndex = 1, texIndex = 2, tangentIndex = 3;
	constexpr GLint valuesPerVertex = 3;

	mesh.vertexArray.SetIndexBuffer(mesh.elementBuffer);

	VertexBufferFormat vbFormat;
	vbFormat.bindingIndex = vertexBinding;
	vbFormat.offset = (GLintptr) nullptr;
	vbFormat.stride = sizeof(Vertex);
	mesh.vertexArray.AddVertexBuffer(mesh.vertexBuffer, vbFormat);

	VertexArrayAttribute attrib;
	attrib.attributeIndex = positionIndex;
	attrib.vertexBindingIndex = vertexBinding;

	attrib.format.normalized = GL_FALSE;
	attrib.format.type = GL_FLOAT;
	attrib.format.size = valuesPerVertex;
	attrib.format.relativeOffset = offsetof(Vertex, Vertex::position);

	mesh.vertexArray.AddAttribute(attrib);

	attrib.format.relativeOffset = offsetof(Vertex, Vertex::normal);
	attrib.attributeIndex = normalIndex;

	mesh.vertexArray.AddAttribute(attrib);

	attrib.format.relativeOffset = offsetof(Vertex, Vertex::textureCoordinate);
	attrib.attributeIndex = texIndex;

	mesh.vertexArray.AddAttribute(attrib);

	attrib.format.relativeOffset = offsetof(Vertex, Vertex::tangent);
	attrib.attributeIndex = tangentIndex;

	mesh.vertexArray.AddAttribute(attrib);

	mesh.vertexArray.EnableAttribute(positionIndex);
	mesh.vertexArray.EnableAttribute(normalIndex);
	mesh.vertexArray.EnableAttribute(texIndex);
	mesh.vertexArray.EnableAttribute(tangentIndex);

	return mesh;
}

Mesh::Mesh(Mesh&& mesh){
	vertexBuffer = std::move(mesh.vertexBuffer);

	elementBuffer = std::move(mesh.elementBuffer);

	vertexArray = std::move(mesh.vertexArray);

	vertices = std::move(mesh.vertices);
	mesh.vertices.clear();

	indices = std::move(mesh.indices);
	mesh.indices.clear();

	material = std::move(mesh.material);
	mesh.material.textures.clear();
}

Mesh & Mesh::operator=(Mesh&& mesh){
	vertexBuffer = std::move(mesh.vertexBuffer);

	elementBuffer = std::move(mesh.elementBuffer);

	vertexArray = std::move(mesh.vertexArray);

	vertices = std::move(mesh.vertices);
	mesh.vertices.clear();

	indices = std::move(mesh.indices);
	mesh.indices.clear();

	material = std::move(mesh.material);
	mesh.material.textures.clear();

	return *this;
}

GLuint Mesh::GetVertexBuffer() const{
	return vertexBuffer.GetId();
}

GLuint Mesh::GetElementBuffer() const{
	return elementBuffer.GetId();
}

GLuint Mesh::GetVertexArray() const{
	return vertexArray.GetId();
}

GLsizei Mesh::GetIndicesCount() const{
	return indices.size();
}

void Mesh::Delete(){
}
