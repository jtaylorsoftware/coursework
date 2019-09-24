/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/


#include "opengl/cubemap.hpp"

#include "engine/euler_camera.hpp"

#include "opengl/shader_program.hpp"
#include "utility/image_utility.hpp"

GLuint Cubemap::vao = 0;
GLuint Cubemap::vbo = 0;
GLuint Cubemap::baseTexture = 0; // special CUBE_MAP target texture for rendering in the scene
GLuint Cubemap::convolvedTexture = 0; // CUBE_MAP target texture for using as irradiance map
GLuint Cubemap::prefilteredTexture = 0; 
GLuint Cubemap::fbo = 0; // Framebuffer for rendering convolution
GLuint Cubemap::rbo = 0; // Renderbuffer for result of convolution
std::shared_ptr<ShaderProgram> Cubemap::renderShader{};
std::shared_ptr<ShaderProgram> Cubemap::convolveShader{};
std::shared_ptr<ShaderProgram> Cubemap::prefilterShader{};
glm::mat4 renderProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

GLfloat cubeVertices[] = {
	-99.0f,  99.0f, -99.0f,
	-99.0f, -99.0f, -99.0f,
	99.0f, -99.0f, -99.0f,
	99.0f, -99.0f, -99.0f,
	99.0f,  99.0f, -99.0f,
	-99.0f,  99.0f, -99.0f,

	-99.0f, -99.0f,  99.0f,
	-99.0f, -99.0f, -99.0f,
	-99.0f,  99.0f, -99.0f,
	-99.0f,  99.0f, -99.0f,
	-99.0f,  99.0f,  99.0f,
	-99.0f, -99.0f,  99.0f,

	99.0f, -99.0f, -99.0f,
	99.0f, -99.0f,  99.0f,
	99.0f,  99.0f,  99.0f,
	99.0f,  99.0f,  99.0f,
	99.0f,  99.0f, -99.0f,
	99.0f, -99.0f, -99.0f,

	-99.0f, -99.0f,  99.0f,
	-99.0f,  99.0f,  99.0f,
	99.0f,  99.0f,  99.0f,
	99.0f,  99.0f,  99.0f,
	99.0f, -99.0f,  99.0f,
	-99.0f, -99.0f,  99.0f,

	-99.0f,  99.0f, -99.0f,
	99.0f,  99.0f, -99.0f,
	99.0f,  99.0f,  99.0f,
	99.0f,  99.0f,  99.0f,
	-99.0f,  99.0f,  99.0f,
	-99.0f,  99.0f, -99.0f,

	-99.0f, -99.0f, -99.0f,
	-99.0f, -99.0f,  99.0f,
	99.0f, -99.0f, -99.0f,
	99.0f, -99.0f, -99.0f,
	-99.0f, -99.0f,  99.0f,
	99.0f, -99.0f,  99.0f
};

glm::mat4 renderViews[] =
{
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

Cubemap::Cubemap(){
}


Cubemap::~Cubemap(){
}


void Cubemap::CreateFromTextures(const std::vector<std::string>& textureNames){
	glCreateTextures(GL_TEXTURE_CUBE_MAP, (GLsizei)1, &baseTexture);
	if (textureNames.size() == 6){
		glBindTexture(GL_TEXTURE_CUBE_MAP, baseTexture);
		for (int i = 0; i < 6; ++i){
			bool flipVertically = (!(i == 2 || i == 3)) ? false : true;
			auto img = ImageLoader::LoadImage(textureNames[i], flipVertically);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
				img.width, img.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
			ImageLoader::FreeImage(img);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, baseTexture);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTextureParameteri(baseTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(baseTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(baseTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(baseTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(baseTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glCreateBuffers(1, &vbo);
	glCreateVertexArrays(1, &vao);
	glNamedBufferData(vbo, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, 3 * sizeof(GLfloat));
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao, 0, 0);
	glEnableVertexArrayAttrib(vao, 0);


	glCreateFramebuffers(1, &fbo);
	glCreateRenderbuffers(1, &rbo);

	glNamedRenderbufferStorage(rbo, GL_DEPTH_COMPONENT24, 32, 32);
	glNamedFramebufferRenderbuffer(fbo, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, rbo);

	GLenum fboStatus = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "FBO incomplete\n";
		exit(-1);
	}


	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &convolvedTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, convolvedTexture);
	for (int i = 0; i < 6; ++i){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTextureParameteri(convolvedTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(convolvedTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(convolvedTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(convolvedTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(convolvedTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &prefilteredTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredTexture);
	for (int i = 0; i < 6; ++i){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTextureParameteri(prefilteredTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(prefilteredTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(prefilteredTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(prefilteredTexture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTextureParameteri(prefilteredTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	

	renderShader = ShaderProgram::CreateFromFile("cubemap");
	convolveShader = ShaderProgram::CreateFromFile("convolve_cubemap");
	prefilterShader = ShaderProgram::CreateFromFile("prefilter_cubemap");
}

GLuint Cubemap::Convolve(){
	GLint viewportDims[4];
	glGetIntegerv(GL_VIEWPORT, viewportDims); // save current viewport dims

	glUseProgram(convolveShader->GetId());
	glProgramUniformMatrix4fv(convolveShader->GetId(), convolveShader->GetUniform("uRenderProjection"),
		1, GL_FALSE, glm::value_ptr(renderProjection));


	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, baseTexture);

	// Render the cubemap once per render view 
	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	for (int i = 0; i < 6; ++i){
		glProgramUniformMatrix4fv(convolveShader->GetId(), convolveShader->GetUniform("uRenderView"),
			1, GL_FALSE, glm::value_ptr(renderViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, convolvedTexture, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	glViewport(viewportDims[0], viewportDims[1], viewportDims[2], viewportDims[3]); // restore viewport

	return convolvedTexture;
}

GLuint Cubemap::Prefilter(){
	GLint viewportDims[4];
	glGetIntegerv(GL_VIEWPORT, viewportDims); // save current viewport dims

	glUseProgram(prefilterShader->GetId());
	glProgramUniformMatrix4fv(prefilterShader->GetId(), prefilterShader->GetUniform("uRenderProjection"),
		1, GL_FALSE, glm::value_ptr(renderProjection));

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, baseTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	int mipLevels = 5;
	for (int level = 0; level < mipLevels; ++level){
		auto mipScale = std::pow(0.5, level);
		int mipWidth = 128 * mipScale;
		int mipHeight = 128 * mipScale;
		
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = static_cast<float>(level) / (mipLevels - 1);
		glProgramUniform1f(prefilterShader->GetId(), prefilterShader->GetUniform("uRoughness"), roughness);
		for (int faceIdx = 0; faceIdx < 6; ++faceIdx){
			glProgramUniformMatrix4fv(prefilterShader->GetId(),
				prefilterShader->GetUniform("uRenderView"), 1, GL_FALSE, glm::value_ptr(renderViews[faceIdx]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIdx, prefilteredTexture, level);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}


	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glViewport(viewportDims[0], viewportDims[1], viewportDims[2], viewportDims[3]); // restore viewport
	return prefilteredTexture;
}

GLuint Cubemap::GetConvolvedMap(){
	return convolvedTexture;
}

GLuint Cubemap::GetPrefilteredMap(){
	return prefilteredTexture;
}

void Cubemap::Render(){
	extern EulerCamera camera;
	glBindVertexArray(vao);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glUseProgram(renderShader->GetId());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, baseTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}
