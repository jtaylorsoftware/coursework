/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/model.hpp"

Model::Model() : name(""), meshes(){
}

Model::Model(const std::string & name, std::vector<Mesh*>& meshes)
	:name(name) {
	for (auto mesh : meshes){
		this->meshes.emplace_back(new Mesh(std::move(*mesh)));
	}
}
