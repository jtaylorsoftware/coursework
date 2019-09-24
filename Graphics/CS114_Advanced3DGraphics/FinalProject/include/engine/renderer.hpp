/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "opengl/shader_program.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <memory>
#include <string>

class Texture;

// Abstract base class that implements some of the functionality for
// a class that allows rendering an object to the screen
class Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	Renderer(const Renderer&) = default;
	Renderer(Renderer&&) = default;

	Renderer& operator=(const Renderer&) = default;
	Renderer& operator=(Renderer&&) = default;

	virtual void Init() = 0; // set static uniforms
	virtual void Update(float deltaTime) = 0; // update per-frame values
	virtual void Render() = 0; // render to the screen
	virtual void RenderToShadowMap() = 0; // render to shadow map

	// These 3D transformation functions affect the renderable object (Mesh) 
	// attached to the Renderer
	void Scale(float scale);
	void Rotate(float radians, const glm::vec3& axis);
	void Translate(const glm::vec3& amount);

	static GLuint GetVPUniformBlock();

	static std::shared_ptr<ShaderProgram> shadowMapProgram;
	static std::unique_ptr<Texture> shadowMapTexture;
	static GLuint shadowMapFramebuffer;
protected:
	static GLuint vpBlock;

	glm::mat4 modelToWorld;
	glm::mat3 normalMatrix;
	glm::vec4 position;
};
