/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/renderer.hpp"

#include "opengl/texture2d.hpp"
#include "utility/image_utility.hpp"
#include "opengl/shader_program.hpp"

#include <iostream>

GLuint Renderer::vpBlock = 0;

std::shared_ptr<ShaderProgram> Renderer::shadowMapProgram{};
std::unique_ptr<Texture> Renderer::shadowMapTexture{};
GLuint Renderer::shadowMapFramebuffer{0};


Renderer::Renderer(){
	// lazy init static members once (needs reworking)
	if (vpBlock == 0 || shadowMapFramebuffer == 0){
		// Init vpBlock
		glGenBuffers(1, &vpBlock);
		glBindBuffer(GL_UNIFORM_BUFFER, vpBlock);
		glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, vpBlock);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		// Init shadow map program
		shadowMapProgram = ShaderProgram::CreateFromFile("shadow_map");

		// Init shadow map framebuffer
		glCreateFramebuffers(1, &shadowMapFramebuffer);
		TextureStorageFormat format;
		format.width = 1024;
		format.height = 1024;
		format.internalFormat = GL_DEPTH_COMPONENT32F;
		format.levels = 1;
		shadowMapTexture.reset(new Texture2d());
		
		shadowMapTexture->AllocateStorage(format);

		shadowMapTexture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		shadowMapTexture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glNamedFramebufferTexture(shadowMapFramebuffer, GL_DEPTH_ATTACHMENT, shadowMapTexture->GetId(), 0);
		glNamedFramebufferDrawBuffer(shadowMapFramebuffer, GL_NONE);
		glNamedFramebufferReadBuffer(shadowMapFramebuffer, GL_NONE);

		GLenum fboStatus = glCheckNamedFramebufferStatus(shadowMapFramebuffer, GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
			std::cout << "Shadow map FBO incomplete\n";
			exit(-1);
		}
	}
}

Renderer::~Renderer(){
}

void Renderer::Scale(float scale){
	modelToWorld = glm::scale(modelToWorld, glm::vec3(scale));
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelToWorld)));
}

void Renderer::Rotate(float radians, const glm::vec3 & axis){
	modelToWorld = glm::rotate(modelToWorld, radians, axis);
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelToWorld)));
}

void Renderer::Translate(const glm::vec3 & amount){
	modelToWorld = glm::translate(modelToWorld, amount);
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelToWorld)));
}

GLuint Renderer::GetVPUniformBlock(){
	return vpBlock;
}
