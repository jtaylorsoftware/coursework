/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/model_library.hpp"

#include "engine/texture_library.hpp"

std::map<std::string, std::shared_ptr<Model>> ModelLibrary::models;

// Map my texture types to the indices I use across shaders
std::unordered_map<TextureType, GLuint> bindingIndexes = {
	{TextureType::Diffuse, 0},
	{TextureType::Normal, 1},
	{TextureType::Specular_Roughness, 2},
	{TextureType::Metallic, 3},
	{TextureType::Ambient, 4},
	{TextureType::Displacement, 5}, // displacement or height
	{TextureType::Emissive, 7},
	{TextureType::Unknown, -1}
};

void ModelLibrary::LoadModel(const std::string& name, const RawModel* rawModel){
	if (rawModel == nullptr){
		return;
	}

	Model* model = new Model();
	model->name = name;
	for (size_t meshIndex = 0; meshIndex < rawModel->meshes.size(); ++meshIndex){
		Mesh mesh = Mesh::Create(rawModel->meshes[meshIndex].vertices, rawModel->meshes[meshIndex].indices);

		Material material;
		for (size_t texIndex = 0; texIndex < rawModel->meshes[meshIndex].textures.size(); ++texIndex){
			const RawTexture& meshTexture = rawModel->meshes[meshIndex].textures[texIndex];

			std::shared_ptr<Texture> texture;
			if (meshTexture.type == TextureType::Diffuse){
				texture = TextureLibrary::LoadTexture2dSrgb(meshTexture.path);
			} else{
				texture = TextureLibrary::LoadTexture2d(meshTexture.path);
			}

			material.textures[bindingIndexes[meshTexture.type]] = texture;
		}
		mesh.material = std::move(material);

		model->meshes.emplace_back(new Mesh(std::move(mesh)));
	}
	models[name].reset(model);
}

std::shared_ptr<Model> ModelLibrary::GetModel(const std::string & name){
	return models[name];
}
