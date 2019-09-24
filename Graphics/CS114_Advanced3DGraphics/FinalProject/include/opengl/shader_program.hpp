/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include "glad/glad.h"
#include "opengl/shader.hpp"

#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

class ShaderProgramError : std::runtime_error
{
public:
	ShaderProgramError(const std::string& message);
	const char * what() const;
};

// Wraps OpenGL shader program functionality, using modern Direct State Access function calls
class ShaderProgram
{
public:
	ShaderProgram();

	ShaderProgram(ShaderProgram&& program); 
	
	~ShaderProgram();

	
	ShaderProgram& operator=(ShaderProgram&& program);


	// Creates a "standard" VS-FS shader from the VS-FS shaders with the same root filepath
	static std::shared_ptr<ShaderProgram> CreateFromFile(const std::string& name);

	inline GLuint GetId() const;

	inline GLuint GetUniform(const std::string& name);

	inline bool IsSeparable() const;

	void AttachShader(const Shader& shader);
	void DetachShader(const Shader& shader);

	void Link();

private:
	void Delete();


	// Allocates memory for the shader but create any shaders from files
	static ShaderProgram Create();

	// Previously loaded programs 
	static std::map<std::string, std::shared_ptr<ShaderProgram>> programs;

	bool IsValid() const;
	std::string GetInfoLog() const;
	GLint GetLogLength() const;

	void ThrowShaderProgramError(const std::string& reason);

	void GetAllInputResourceProperties();
	void GetAllUniformLocations();
	void GetAllUniformNames();

	// Ensure locations are defaulted to -1 for invalid locations by using a struct with default value
	struct LocationValue
	{
		operator GLint() const{ return location; }
		GLint location = -1;
	};

	std::map<std::string, LocationValue> GetAllResourceNames(GLenum resource);
	LocationValue GetResourceLocation(GLenum resource, const std::string& name);

	std::map<std::string, LocationValue> uniforms;

	GLuint id;
	bool isSeparable;
};

GLuint ShaderProgram::GetId() const{
	return id;
}

bool ShaderProgram::IsSeparable() const{
	return isSeparable;
}

GLuint ShaderProgram::GetUniform(const std::string & name){
	return uniforms[name];
}
