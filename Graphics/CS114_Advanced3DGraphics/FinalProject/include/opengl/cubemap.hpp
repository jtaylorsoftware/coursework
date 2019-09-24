/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "glad/glad.h"

#include <memory>
#include <vector>

class ShaderProgram;

// Static class for now (one cubemap per scene); it might make more sense to make
// it possible to have multiple cubemap instances later.
class Cubemap
{
public:
	Cubemap();
	~Cubemap();

	// Create a cubemap from a set of 6 textures. They should be in the order right,left,top,bottom,back,front
	static void CreateFromTextures(const std::vector<std::string>& textureNames);

	static GLuint Convolve();
	static GLuint Prefilter();
	static GLuint GetConvolvedMap();
	static GLuint GetPrefilteredMap();
	static void Render();
private:
	static GLuint vao, vbo;
	static GLuint baseTexture; // special CUBE_MAP target texture for rendering in the scene
	static GLuint convolvedTexture; // CUBE_MAP target texture for using as diffuse irradiance map
	static GLuint prefilteredTexture; // CUBE_MAP target texture for prefiltered environment map (used for specular)
	static GLuint fbo; // Framebuffer for rendering convolution, prefilter
	static GLuint rbo; // Renderbuffer for result of convolution
	static std::shared_ptr<ShaderProgram> renderShader;
	static std::shared_ptr<ShaderProgram> convolveShader;
	static std::shared_ptr<ShaderProgram> prefilterShader;
};