/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/
#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "opengl/shader.hpp"
#include "opengl/shader_program.hpp"
#include "opengl/texture.hpp"

#include <memory>
#include <unordered_map>
#include <utility>

// Simple class that stores a map of textures and a shader to be used
// for rendering a mesh
class Material
{
public:
	Material();
	~Material();

	Material(Material&& material);

	Material& operator=(Material&& material);

	std::unordered_map<GLuint, std::shared_ptr<Texture>> textures; // Map of shader binding index -> texture
	std::shared_ptr<ShaderProgram> shaderProgram;
private:
};
