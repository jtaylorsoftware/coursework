/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "engine/model.hpp"
#include "engine/renderer.hpp"

// Core class that represents an object in the world (scene)
// Anything that is rendered into the world is going to be a Model attached
// to a WorldObject, at least for this program. 
class WorldObject
{
public:
	WorldObject();
	virtual ~WorldObject();

	void Init(); // set any static values
	void Update(float deltaTime); // update per-frame values

	void Scale(float scale);
	void Rotate(float radians, const glm::vec3& axis);
	void Translate(const glm::vec3& amount);

	void Render();
	void RenderToShadowMap();

	std::shared_ptr<Model> GetModel() const;
	void SetModel(std::shared_ptr<Model> model);

protected:
	std::shared_ptr<Model> model;

	// List of renderers to render each submesh of the model.
	// should realistically be one renderer/object (multi-mesh then requires multi-worldobject)
	std::vector<std::unique_ptr<Renderer>> renderers; 
};

