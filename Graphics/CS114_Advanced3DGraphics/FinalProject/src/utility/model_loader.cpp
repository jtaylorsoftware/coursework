/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "utility/model_loader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "utility/assimp_model_loader.hpp"

RawModel* ModelLoader::LoadModelFromExternalFileFormat(const std::string & path){
	AssimpModelLoader loader;
	RawModel* model = loader.LoadModelFromFile(path);
	return model;
}
void ModelLoader::SaveToBinaryFile(const RawModel * const model, const std::string& path){
	// Naively saves out some model data to a binary file just by writing out some data in a specific order.
	// Uses sizes of the mesh data to get around.
	std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);
	if (file.good()){
		// Write out the number of meshes in the file
		unsigned numMeshes = static_cast<unsigned>(model->meshes.size());
		file.write(reinterpret_cast<const char*>(&numMeshes), sizeof(numMeshes));

		// Write per mesh data
		auto& meshes = model->meshes;
		for (unsigned i = 0; i < numMeshes; ++i){
			unsigned numVertices = static_cast<unsigned>(meshes[i].vertices.size());
			unsigned numIndices = static_cast<unsigned>(meshes[i].indices.size());
			unsigned numTextures = static_cast<unsigned>(meshes[i].textures.size());

			// Write the number of vertices, indices, and textures
			file.write(reinterpret_cast<const char*>(&numVertices), sizeof(numVertices));
			file.write(reinterpret_cast<const char*>(&numIndices), sizeof(numIndices));
			file.write(reinterpret_cast<const char*>(&numTextures), sizeof(numTextures));

			// Write the vertices and indices data to the file
			file.write(reinterpret_cast<const char*>(meshes[i].vertices.data()), numVertices * sizeof(Vertex));
			file.write(reinterpret_cast<const char*>(meshes[i].indices.data()), numIndices * sizeof(unsigned));

			// For every texture write the type of the texture, the size of its path, and then the path itself
			auto& textures = meshes[i].textures;
			for (unsigned j = 0; j < numTextures; ++j){
				file.write(reinterpret_cast<const char*>(&textures[j].type), sizeof(TextureType));

				unsigned pathSize = static_cast<unsigned>(textures[j].path.size());
				file.write(reinterpret_cast<const char*>(&pathSize), sizeof(unsigned));
				file.write(reinterpret_cast<const char*>(textures[j].path.c_str()), textures[j].path.size());
			}
		}
		file.close();
	}
}
RawModel * ModelLoader::LoadModelFromBinaryFile(const std::string & path){
	// Loads the binary model file using more or less the reverse process of saving
	RawModel* model = nullptr;

	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (file.good()){
		model = new RawModel();
		unsigned numMeshes = 0;
		file.read(reinterpret_cast<char*>(&numMeshes), sizeof(unsigned));
		model->meshes.resize(numMeshes);

		for (unsigned i = 0; i < numMeshes; ++i){
			unsigned numVertices = 0;
			file.read(reinterpret_cast<char*>(&numVertices), sizeof(unsigned));

			unsigned numIndices = 0;
			file.read(reinterpret_cast<char*>(&numIndices), sizeof(unsigned));

			unsigned numTextures = 0;
			file.read(reinterpret_cast<char*>(&numTextures), sizeof(unsigned));

			model->meshes[i].vertices.resize(numVertices);
			file.read(reinterpret_cast<char*>(&model->meshes[i].vertices[0]), numVertices * sizeof(Vertex));

			model->meshes[i].indices.resize(numIndices);
			file.read(reinterpret_cast<char*>(&model->meshes[i].indices[0]), numIndices * sizeof(unsigned));

			model->meshes[i].textures.resize(numTextures);
			for (unsigned j = 0; j < numTextures; ++j){
				file.read(reinterpret_cast<char*>(&model->meshes[i].textures[j].type), sizeof(TextureType));

				unsigned pathSize = 0;
				file.read(reinterpret_cast<char*>(&pathSize), sizeof(unsigned));
				model->meshes[i].textures[j].path.resize(pathSize);
				file.read(reinterpret_cast<char*>(&model->meshes[i].textures[j].path[0]), pathSize);
			}
		}

		file.close();
	}

	return model;
}


