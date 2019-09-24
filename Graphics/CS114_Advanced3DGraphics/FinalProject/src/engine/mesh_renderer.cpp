/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/
#include "engine/mesh_renderer.hpp"

#include "engine/light_manager.hpp"
#include "engine/euler_camera.hpp"
#include "engine/material.hpp"
#include "engine/mesh.hpp"

#include "opengl/texture.hpp"

MeshRenderer::MeshRenderer(Mesh * mesh){
	this->mesh = mesh;
}

MeshRenderer::~MeshRenderer(){

}

void MeshRenderer::SetMesh(Mesh * mesh){
}

void MeshRenderer::Init(){

	modelToWorld = glm::mat4(1.f);
	normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelToWorld)));

}


void MeshRenderer::Update(float deltaTime){
	
}

void MeshRenderer::Render(){
	glUseProgram(mesh->material.shaderProgram->GetId());

	UpdateUniforms(&mesh->material);

	if (shadowMapTexture != nullptr){
		glBindTextureUnit(9, shadowMapTexture->GetId());
	}

	for (size_t j = 0; j < mesh->material.textures.size(); ++j){
		glBindTextureUnit(j, mesh->material.textures[j]->GetId());
	}

	glBindVertexArray(mesh->GetVertexArray());
	glDrawElements(GL_TRIANGLES, mesh->GetIndicesCount(), GL_UNSIGNED_INT, 0);
}

void MeshRenderer::RenderToShadowMap(){
	auto lightSpaceMat = LightManager::GetDirectionalLightProjectionMat() * LightManager::GetDirectionalLightView();
	glProgramUniformMatrix4fv(shadowMapProgram->GetId(),
		shadowMapProgram->GetUniform("uModel"),
		1, GL_FALSE, glm::value_ptr(modelToWorld));
	glProgramUniformMatrix4fv(shadowMapProgram->GetId(),
		shadowMapProgram->GetUniform("uLightSpaceMat"),
		1, GL_FALSE, glm::value_ptr(lightSpaceMat));

	glUseProgram(shadowMapProgram->GetId());


	glBindVertexArray(mesh->GetVertexArray());
	glDrawElements(GL_TRIANGLES, mesh->GetIndicesCount(), GL_UNSIGNED_INT, 0);
}

void MeshRenderer::UpdateUniforms(Material* material){
	extern EulerCamera camera;
	auto lightSpaceMat = LightManager::GetDirectionalLightProjectionMat() * LightManager::GetDirectionalLightView();

	glProgramUniform3fv(material->shaderProgram->GetId(),
		material->shaderProgram->GetUniform("dirLightDirection"),
		1, glm::value_ptr(LightManager::GetDirectionalLight().direction));
	glProgramUniform3fv(material->shaderProgram->GetId(),
		material->shaderProgram->GetUniform("uViewPos"),
		1, glm::value_ptr(camera.GetPosition()));
	glProgramUniformMatrix4fv(material->shaderProgram->GetId(),
		material->shaderProgram->GetUniform("uModel"),
		1, GL_FALSE, glm::value_ptr(modelToWorld));
	glProgramUniformMatrix4fv(material->shaderProgram->GetId(),
		material->shaderProgram->GetUniform("uLightSpaceMat"),
		1, GL_FALSE, glm::value_ptr(lightSpaceMat));
	glProgramUniformMatrix3fv(material->shaderProgram->GetId(),
		material->shaderProgram->GetUniform("uNormalMatrix"),
		1, GL_FALSE, glm::value_ptr(normalMatrix));
}
