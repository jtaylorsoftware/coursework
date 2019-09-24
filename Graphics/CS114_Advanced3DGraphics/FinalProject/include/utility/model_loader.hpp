/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "utility/model_types.hpp"

#include <string>
#include <vector>

// Static class that has utility functions for loading models from either my own binary .model format
// or from other external formats by using assimp
class ModelLoader{
public:
	static RawModel* LoadModelFromExternalFileFormat(const std::string& path);

	static void SaveToBinaryFile(const RawModel* const model, const std::string& path);
	static RawModel* LoadModelFromBinaryFile(const std::string& path);
};

