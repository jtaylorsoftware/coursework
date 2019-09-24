/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/


#include "opengl/shader.hpp"

#include <sstream>

#include "utility/utility.hpp"

Shader::Shader() : id(0){
}

Shader::Shader(Shader&& shader){
	id = shader.id;
	shader.id = 0;
}

Shader::~Shader(){
	Delete();
}

Shader & Shader::operator=(Shader&& shader){
	id = shader.id;
	shader.id = 0;
	return *this;
}

void Shader::Delete(){
	if (id != 0){
		glDeleteShader(id);
	}
}

Shader Shader::CreateFromFile(GLenum type, const std::string & filePath){
	std::string source = ReadFile(filePath);
	Shader shader = CreateFromSource(type, source);
	return shader;
}

Shader Shader::CreateFromSource(GLenum type, const std::string & source){
	Shader shader;
	shader.id = glCreateShader(type);
	if (shader.id == 0){
		shader.ThrowShaderError("glCreateShader returned 0", type);
	}

	glShaderSource(shader.id, 1, GLString(source), NULL);
	glCompileShader(shader.id);

	if (!shader.IsValid()){
		shader.ThrowShaderError("glCompileShader failed", type);
	}

	return std::move(shader);
}

bool Shader::IsValid() const{
	GLint isCompiled;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	return isCompiled == GL_TRUE;
}

std::string Shader::GetInfoLog() const{
	std::string log;

	if (id != 0){
		GLsizei logLength = GetLogLength();
		log.resize(logLength);
		glGetShaderInfoLog(id, logLength, &logLength, &log[0]);
	}

	return log;
}

GLint Shader::GetLogLength() const{
	GLint logLength = 0;

	if (id != 0){
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
	}

	return logLength;

}

void Shader::ThrowShaderError(const std::string& reason, GLenum type) const{
	std::ostringstream error;
	if (id != 0){
		error << "Failed to create shader of type: " << OpenGlEnumToString(type) << ", " << reason << ".\n"
			<< "Shader Log:\n" << GetInfoLog();
	} else{
		error << "Failed to create shader: " << reason << ".";
	}

	throw ShaderError(error.str());
}

ShaderError::ShaderError(const std::string & message) : runtime_error(message){
}

const char * ShaderError::what() const{
	return runtime_error::what();
}
