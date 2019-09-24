/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/


#pragma once

#include <string>
#include <stdexcept>

#include "glad/glad.h"

class ShaderError : std::runtime_error
{
public:
	ShaderError(const std::string& message);
	const char* what() const;
};

// Wraps OpenGL shader objects, using DSA for bindless state updates
class Shader
{
public:
	Shader();
	Shader(Shader&& shader);

	~Shader();

	Shader& operator=(Shader&& shader);

	void Delete();

	static Shader CreateFromFile(GLenum type, const std::string& filePath);
	static Shader CreateFromSource(GLenum type, const std::string& source);

	inline GLuint GetId() const;
private:
	bool IsValid() const;
	std::string GetInfoLog() const;
	GLint GetLogLength() const;

	void ThrowShaderError(const std::string& reason, GLenum type) const;

	GLuint id;
};

GLuint Shader::GetId() const{
	return id;
}
