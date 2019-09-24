/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "utility/assimp_model_loader.hpp"

#include <iostream>
#include <cassert>
#include <set>
#include <sstream>
#include <stdexcept>

#include "assimp/texture.h"
#include "assimp/postprocess.h"

AssimpModelLoader::AssimpModelLoader() : model(nullptr){
}

AssimpModelLoader::~AssimpModelLoader(){
}

RawModel * AssimpModelLoader::LoadModelFromFile(const std::string & path){
	filePath = path;

	// try to find the current directory
	auto lastFwdSlash = filePath.find_last_of('/');
	auto lastBackSlash = filePath.find_last_of('\\');

	size_t lastSlash = 0;  // assume name of file is good enough if there's no slashes found (are in immediate directory)

	if (lastFwdSlash == std::string::npos){ // prefer using last fwd slash to find start of name
		if (lastBackSlash != std::string::npos){ // fall back to last back slash
			lastSlash = lastBackSlash;
		}
	} else{
		lastSlash = lastFwdSlash;
	}

	directory = filePath.substr(0, lastSlash);

	// add slash if directory is not the immediate one
	if (directory.size() > 0){
		directory.append("/");
	}

	//get the file extension
	auto lastPeriod = path.find_last_of(".");
	if (lastPeriod != std::string::npos){
		fileExtension = path.substr(lastPeriod + 1);
	}

	LoadModel();
	return model;
}

void AssimpModelLoader::LoadModel(){
	model = new RawModel();

	CreateAssimpModules();

	
	ProcessSceneNodes(assimp.scene->mRootNode);
}

void AssimpModelLoader::CreateAssimpModules(){
	assimp.importer = new Assimp::Importer();
	assimp.importer->SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS | aiComponent_LIGHTS | aiComponent_CAMERAS);
	assimp.importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

	assimp.scene = assimp.importer->ReadFile(filePath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace | aiProcess_RemoveComponent);
	if (assimp.scene == nullptr){
		const char* error = assimp.importer->GetErrorString();
		throw std::runtime_error(error);
	}
}

void AssimpModelLoader::ProcessSceneNodes(aiNode * const inNode){
	// For every child node of the root node, process the aiMeshes associated with the node.
	model->meshes.reserve(model->meshes.capacity() + inNode->mNumMeshes);
	for (unsigned int meshIndex = 0; meshIndex < inNode->mNumMeshes; ++meshIndex){
		aiMesh* mesh = assimp.scene->mMeshes[inNode->mMeshes[meshIndex]];
		RawMesh modelMesh;
		model->meshes.push_back(ProcessMesh(mesh));
	}

	for (unsigned int childIndex = 0; childIndex < inNode->mNumChildren; ++childIndex){
		ProcessSceneNodes(inNode->mChildren[childIndex]);
	}
}

RawMesh AssimpModelLoader::ProcessMesh(aiMesh * const inAiMesh){
	// Grab the vertex and texture data for a mesh and transform into my data format

	RawMesh outMesh;
	outMesh.vertices.reserve(inAiMesh->mNumFaces);
	
	for (unsigned int vertexIndex = 0; vertexIndex < inAiMesh->mNumVertices; ++vertexIndex){
		outMesh.vertices.push_back(ProcessVertex(inAiMesh, vertexIndex));
	}

	outMesh.indices = ProcessFaceIndices(inAiMesh);

	aiMaterial* const material = assimp.scene->mMaterials[inAiMesh->mMaterialIndex];

	outMesh.textures = ProcessMaterials(material);

	return outMesh;
}

Vertex AssimpModelLoader::ProcessVertex(aiMesh * const inAiMesh, const unsigned int vertexIndex){
	Vertex vertex;

	// Get positions, normals, texture coords, tangents for the vertex of a mesh
	if (inAiMesh->HasPositions()){
		vertex.position = AiVector3dToVec3f(inAiMesh->mVertices[vertexIndex]);
	}
	if (inAiMesh->HasNormals()){
		vertex.normal = AiVector3dToVec3f(inAiMesh->mNormals[vertexIndex]);
	}
	constexpr unsigned int textureCoordinateLayerIndex = 0;
	if (inAiMesh->HasTextureCoords(textureCoordinateLayerIndex)){
		vertex.textureCoordinate = AiVector3dToVec2f(inAiMesh->mTextureCoords[textureCoordinateLayerIndex][vertexIndex]);
	}
	if (inAiMesh->HasTangentsAndBitangents()){
		vertex.tangent = AiVector3dToVec3f(inAiMesh->mTangents[vertexIndex]);
	}

	return vertex;
}

std::vector<unsigned int> AssimpModelLoader::ProcessFaceIndices(aiMesh* const inAiMesh){
	std::vector<unsigned int> indices;

	constexpr unsigned int indicesPerFace = 3;
	indices.reserve(inAiMesh->mNumFaces * indicesPerFace);
	for (unsigned int faceIndex = 0; faceIndex < inAiMesh->mNumFaces; ++faceIndex){
		assert(inAiMesh->mFaces[faceIndex].mNumIndices == indicesPerFace);
		aiFace& face = inAiMesh->mFaces[faceIndex];
		for (unsigned int i = 0; i < face.mNumIndices; ++i){
			indices.push_back(face.mIndices[i]);
		}
	}

	return indices;
}

std::vector<RawTexture> AssimpModelLoader::ProcessMaterials(aiMaterial * const material){

	return GetMaterialTextures(material);
}

std::vector<RawTexture> AssimpModelLoader::GetMaterialTextures(aiMaterial * const material){
	std::vector<RawTexture> textures;
	std::vector<aiTextureType> textureTypes = //aitexturetypes that I suppport 
	{
		aiTextureType_DIFFUSE,
		aiTextureType_AMBIENT,
		aiTextureType_DISPLACEMENT,
		aiTextureType_HEIGHT,
		aiTextureType_NORMALS,
		aiTextureType_SHININESS,
		aiTextureType_SPECULAR, 
		aiTextureType_UNKNOWN
	};
	
	for (auto & type : textureTypes){
 		const unsigned int numTextures = material->GetTextureCount(type);
		for (unsigned int textureIndex = 0; textureIndex < numTextures; ++textureIndex){
			aiString path;
			aiReturn result = material->GetTexture(type, textureIndex, &path);
			assert(result == AI_SUCCESS);

			RawTexture texture;

			// If path returned from Assimp is of the format r"*d+" the texture is embedded. The string
			// is then a path into the scene's texture array.
			// I don't really support embedded textures for now so they probably won't work at all. 
			if (std::string(path.data).find(":") == std::string::npos){
				texture.path = directory + path.data;
			} else{
				texture.path = path.data;
			}

			texture.type = AiTextureTypeToTextureType(type);

			textures.push_back(texture);
		}
	}

	return textures;
}

// Convert aiTextureType to my texture type
TextureType AssimpModelLoader::AiTextureTypeToTextureType(aiTextureType type){
	TextureType textureType = TextureType::Unknown;
	switch (type){
	case aiTextureType_DIFFUSE:
		textureType = TextureType::Diffuse;
		break;
	case aiTextureType_AMBIENT:
		textureType = TextureType::Ambient;
		break;
	case aiTextureType_SPECULAR:
		textureType = TextureType::Specular_Roughness;
		break;
	case aiTextureType_NORMALS:
		textureType = TextureType::Normal;
		break;
	case aiTextureType_DISPLACEMENT:
		textureType = TextureType::Displacement;
		break;
	case aiTextureType_EMISSIVE:
		textureType = TextureType::Emissive;
		break;
	case aiTextureType_HEIGHT:
		// in .obj with .mtl format, normals are loaded as height sometimes apparently
		if (fileExtension == "obj"){
			textureType = TextureType::Normal;
		} else{
			textureType = TextureType::Height;
		}
		break;
	case aiTextureType_SHININESS:
		textureType = TextureType::Metallic;
		break;
	case aiTextureType_UNKNOWN:
		textureType = TextureType::Unknown;
		break;
	}

	return textureType;
}

// Functions to convert aiVectors to my vector type
Vec3f AssimpModelLoader::AiVector3dToVec3f(const aiVector3D& inVertex){
	Vec3f outVec;
	outVec.x = inVertex.x;
	outVec.y = inVertex.y;
	outVec.z = inVertex.z;
	return outVec;
}

Vec2f AssimpModelLoader::AiVector3dToVec2f(const aiVector3D & inVertex){
	Vec2f outVec;
	outVec.x = inVertex.x;
	outVec.y = inVertex.y;
	return outVec;
}

Vec2f AssimpModelLoader::AiVector2dToVec2f(const aiVector2D & inVertex){
	Vec2f outVec;
	outVec.x = inVertex.x;
	outVec.y = inVertex.y;
	return outVec;
}
