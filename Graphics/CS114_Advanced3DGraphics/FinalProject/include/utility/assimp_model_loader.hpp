/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "utility/model_types.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/types.h"

#include <map>
#include <vector>

// Wraps Assimp library functionality to load models into a format usable by my program
class AssimpModelLoader
{
public:
	AssimpModelLoader();
	~AssimpModelLoader();

	RawModel* LoadModelFromFile(const std::string& path);

private:
	void LoadModel();
	void CreateAssimpModules();

	void ProcessSceneNodes(aiNode* const inNode);
	RawMesh ProcessMesh(aiMesh* const inAiMesh);
	static Vertex ProcessVertex(aiMesh* const inAiMesh, const unsigned int vertexIndex);
	static std::vector<unsigned int> ProcessFaceIndices(aiMesh* const inAiMesh);
	std::vector<RawTexture> ProcessMaterials(aiMaterial* const material);
	std::vector<RawTexture> GetMaterialTextures(aiMaterial* const material);

	TextureType AiTextureTypeToTextureType(aiTextureType type);

	static Vec3f AiVector3dToVec3f(const aiVector3D& inVertex);
	static Vec2f AiVector3dToVec2f(const aiVector3D& inVertex);
	static Vec2f AiVector2dToVec2f(const aiVector2D& inVertex);

	struct AssimpModules
	{
		const aiScene* scene = nullptr;
		Assimp::Importer* importer = nullptr;
	};
	AssimpModules assimp;

	RawModel* model;
	std::string filePath;
	std::string fileExtension;
	std::string directory;
};

