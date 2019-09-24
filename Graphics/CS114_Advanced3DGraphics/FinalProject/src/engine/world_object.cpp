/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/world_object.hpp"

#include "engine/mesh_renderer.hpp"

WorldObject::WorldObject(){
}


WorldObject::~WorldObject(){
}

void WorldObject::Init(){
	for (auto& renderer : renderers){
		renderer->Init();
	}
}

void WorldObject::Update(float deltaTime){
	for (auto& renderer : renderers){
		renderer->Update(deltaTime);
	}
}

void WorldObject::Scale(float scale){
	for (auto& renderer : renderers){
		renderer->Scale(scale); // do same scale to all submeshes
	}
}

void WorldObject::Rotate(float radians, const glm::vec3 & axis){
	for (auto& renderer : renderers){
		renderer->Rotate(radians, axis); // do same rotation to all submeshes
	}
}

void WorldObject::Translate(const glm::vec3 & amount){
	for (auto& renderer : renderers){
		renderer->Translate(amount); // do same translation to all submeshes
	}
}

void WorldObject::Render(){
	for (auto& renderer : renderers){
		renderer->Render();
	}
}

void WorldObject::RenderToShadowMap(){
	for (auto& renderer : renderers){
		renderer->RenderToShadowMap();
	}
}

std::shared_ptr<Model> WorldObject::GetModel() const{
	return model;
}

void WorldObject::SetModel(std::shared_ptr<Model> model){
	this->model = model;
	int numMeshes = model->meshes.size();
	renderers.reserve(numMeshes);
	for (int i = 0; i < numMeshes; ++i){
		renderers.emplace_back(new MeshRenderer(model->meshes[i].get()));
	}
}
