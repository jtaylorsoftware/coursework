/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/
#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/ext.hpp"

#include <iostream>

// Camera class that uses euler angles to compute facing direction.
// Could be static in this version, but kept as-is and instead I use
// a global variable. Future versions should use a camera manager class of 
// some kind.
class EulerCamera
{
public:
	EulerCamera(const glm::vec3& position = glm::vec3(0, 3.f, 3.f)) : position(position),
		rotateSpeed(.1f), movementSpeed(.25f),
		pitch(0.f), yaw(0.f), verticalAxisSign(-1.f){
		CalculateViewBasisVectors();
	}

	void Update(float deltaTime){
		CalculateViewBasisVectors();
		view = glm::lookAt(position, position + direction, glm::cross(right, direction));
	}

	void InvertVerticalAxis(){
		verticalAxisSign *= -1;
	}

	float GetPitch(){
		return pitch;
	}

	float GetYaw(){
		return yaw;
	}

	void OffsetPitchFromCurrent(float deltaPitch){
		pitch += deltaPitch * rotateSpeed;
	}

	void OffsetYawFromCurrent(float deltaYaw){
		yaw += deltaYaw * rotateSpeed;
	}

	glm::mat4 GetView(){
		return view;
	}

	void SetPosition(const glm::vec3& position){
		this->position = position;
	}

	glm::vec3 GetPosition(){
		return position;
	}

	glm::vec3 GetDirection(){
		return direction;
	}

	void Translate(const glm::vec3& translation){
		position += translation;
	}

	void Translate(float x, float y, float z){
		position.x += x;
		position.y += y;
		position.z += z;
	}

	void Rotate(float pitch, float yaw){
		pitch = pitch;
		yaw = yaw;
		Rotate();
	}

	void StrafeLeft(){
		position -= right * movementSpeed;
	}
	void StrafeRight(){
		position += right * movementSpeed;
	}
	void MoveForward(){
		position += direction * movementSpeed;
	}
	void MoveBackward(){
		position -= direction * movementSpeed;
	}

	float rotateSpeed, movementSpeed;
private:

	void Rotate(){
		ClampEulerAngles();
	}

	void ClampEulerAngles(){
		float pitchRotationLimit = glm::pi<float>() / 2.f - 0.001f;

		if (pitch > pitchRotationLimit){
			pitch = pitchRotationLimit;
		}
		if (pitch < -pitchRotationLimit){
			pitch = -pitchRotationLimit;
		}

		yaw = glm::mod(yaw, glm::pi<float>() * 2.f);
	}

	void CalculateViewBasisVectors(){
		CalculateFacingDirectionFromAngles();
		CalculateRightVectorFromDirection();
	}

	void CalculateFacingDirectionFromAngles(){
		direction.x = sin(yaw) * cos(pitch);
		direction.y = sin(pitch) * verticalAxisSign;
		direction.z = -cos(yaw) * cos(pitch);
		direction = glm::normalize(direction);
	}

	void CalculateRightVectorFromDirection(){
		right = glm::normalize(glm::cross(direction, glm::vec3(0.f, 1.f, 0.f)));
	}

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 right;
	glm::mat4 view;

	float pitch, yaw;
	float verticalAxisSign;
};

