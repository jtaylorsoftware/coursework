/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/material.hpp"

Material::Material(){
}

Material::~Material(){
}

Material::Material(Material&& material){
	textures = std::move(material.textures);
	material.textures.clear();
}

Material & Material::operator=(Material&& material){
	textures = std::move(material.textures);
	material.textures.clear();

	return *this;
}
