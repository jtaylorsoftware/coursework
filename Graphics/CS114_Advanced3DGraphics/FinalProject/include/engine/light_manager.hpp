/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/
#pragma once


#include "engine/light.hpp"

#include "glad/glad.h"

#include <vector>

// Static class that stores the light data for the scene.
class LightManager
{
public:
	LightManager();
	~LightManager();

	static void Init();

	static void UpdateLightBuffer();
	static GLuint GetLightBuffer();

	static void AddPointLight(PointLight light);
	static void RemovePointLight(unsigned int index);
	static inline PointLight GetPointLightAt(unsigned int index);
	static 	std::vector<PointLight> GetPointLights();
	static inline unsigned int GetPointLightsCount();

	static void SetDirectionalLight(DirectionalLight directionalLight);
	static DirectionalLight GetDirectionalLight();

	// annoyance - DirectionalLight should just be a class with these methods
	static glm::mat4 GetDirectionalLightProjectionMat();
	static glm::mat4 GetDirectionalLightView();
private:
	static std::vector<PointLight> pointLights;
	static DirectionalLight directionalLight;
	static GLuint lightDataBuffer;
};

inline PointLight LightManager::GetPointLightAt(unsigned int index){
	return pointLights[index];
}

inline unsigned int LightManager::GetPointLightsCount(){
	return pointLights.size();
}