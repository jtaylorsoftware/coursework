/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/
#pragma once

#include "engine/renderer.hpp"

class Mesh;
class Material;
class Texture;

// Handles logic for rendering meshes to the screen
class MeshRenderer : public Renderer
{
public:
	MeshRenderer(Mesh* mesh = nullptr);
	~MeshRenderer();

	MeshRenderer(const MeshRenderer&) = default;
	MeshRenderer(MeshRenderer&&) = default;

	MeshRenderer& operator=(const MeshRenderer&) = default;
	MeshRenderer& operator=(MeshRenderer&&) = default;

	void SetMesh(Mesh* mesh); // Set the Mesh that this object should render

	virtual void Init(); // Call before doing any 3D transformations on the Mesh
	virtual void Update(float deltaTime); 
	virtual void Render(); // Handles rendering to the scene using the Mesh's material
	virtual void RenderToShadowMap(); // Handles rendering to a shadow map using the Mesh's vertex data
protected:
private:
	Mesh* mesh;
	void UpdateUniforms(Material* material);
};
