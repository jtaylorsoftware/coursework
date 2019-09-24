/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "engine/mesh.hpp"

#include "glad/glad.h"

#include <memory>
#include <vector>
#include <string>

// Simple class that wraps a collection of Meshes so that a full "model" comprised
// of multiple submeshes can be passed around and rendered. In a more complex system
// WorldObjects may have child WorldObjects with Meshes attached to handle multi-Mesh models.
class Model
{
public:
	Model();
	Model(const std::string& name, std::vector<Mesh*>& meshes);
	std::string name;
	std::vector<std::unique_ptr<Mesh>> meshes;
};