/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/
#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/*
Definitions for light types
*/


struct PointLight
{
	glm::vec4 position;
	glm::vec4 color;
};

struct DirectionalLight
{
	glm::vec4 direction;
	glm::vec4 position;
	glm::vec4 color;
};
