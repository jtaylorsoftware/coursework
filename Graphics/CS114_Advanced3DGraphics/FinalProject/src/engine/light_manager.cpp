/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/light_manager.hpp"

#include "glm/glm.hpp"

std::vector<PointLight> LightManager::pointLights{};
DirectionalLight LightManager::directionalLight{};
GLuint LightManager::lightDataBuffer{0};

LightManager::LightManager(){
}

LightManager::~LightManager(){
}

void LightManager::Init(){
	glGenBuffers(1, &lightDataBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lightDataBuffer);
	glBufferData(GL_UNIFORM_BUFFER, 32, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightDataBuffer);
	auto i = glGetUniformBlockIndex(lightDataBuffer, "Lights");
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::UpdateLightBuffer(){
	glBindBuffer(GL_UNIFORM_BUFFER, lightDataBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(directionalLight.direction));
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec4), glm::value_ptr(directionalLight.color));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

GLuint LightManager::GetLightBuffer(){
	return lightDataBuffer;
}

void LightManager::AddPointLight(PointLight light){
	pointLights.push_back(light);
}

void LightManager::RemovePointLight(unsigned int index){
	pointLights.erase(pointLights.begin() + index);
}

std::vector<PointLight> LightManager::GetPointLights(){
	return pointLights;
}

void LightManager::SetDirectionalLight(DirectionalLight directionalLight){
	LightManager::directionalLight = directionalLight;
}

DirectionalLight LightManager::GetDirectionalLight(){
	return directionalLight;
}

glm::mat4 LightManager::GetDirectionalLightProjectionMat(){
	constexpr float nearPlane = 0.1f;
	constexpr float farPlane = 100.0f;
	return glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
}

glm::mat4 LightManager::GetDirectionalLightView(){
	return glm::lookAt(glm::vec3(directionalLight.position), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
}
