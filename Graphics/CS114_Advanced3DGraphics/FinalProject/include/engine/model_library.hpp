/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include <map>
#include <memory>

#include "engine/model.hpp"

#include "utility/model_types.hpp"

// Static class that stores loaded Model objects so they only needed to be loaded once
// in the program's lifetime
class ModelLibrary
{
public:
	// Loads a Model from raw 3D model file data, and sets up the Mesh objects for the Model.
	static void LoadModel(const std::string& name, const RawModel* rawModel);

	// Gets a Model by name
	static std::shared_ptr<Model> GetModel(const std::string& name); 

private:
	static std::map<std::string, std::shared_ptr<Model>> models;
};
