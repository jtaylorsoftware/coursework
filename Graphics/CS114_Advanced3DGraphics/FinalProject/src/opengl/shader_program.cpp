/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/


#include "opengl/shader_program.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "opengl/shader.hpp"
#include "utility/utility.hpp"

std::map<std::string, std::shared_ptr<ShaderProgram>> ShaderProgram::programs{};

ShaderProgram::ShaderProgram() : id(0), isSeparable(false){
}

ShaderProgram::ShaderProgram(ShaderProgram&& program){
	uniforms = std::move(program.uniforms);
	program.uniforms.clear();

	id = program.id;
	program.id = 0;

	isSeparable = program.isSeparable;
	program.isSeparable = false;
}

ShaderProgram::~ShaderProgram(){
	Delete();
}

ShaderProgram & ShaderProgram::operator=(ShaderProgram&& program){
	uniforms = std::move(program.uniforms);
	program.uniforms.clear();

	id = program.id;
	program.id = 0;

	isSeparable = program.isSeparable;
	program.isSeparable = false;

	return *this;
}

void ShaderProgram::Delete(){
	if (id != 0){
		glDeleteProgram(id);
	}
}

std::shared_ptr<ShaderProgram> ShaderProgram::CreateFromFile(const std::string& name){
	if (programs.count(name) != 0){
		return programs[name];
	}

	ShaderProgram program;
	try{
		Shader vert, frag;
		vert = Shader::CreateFromFile(GL_VERTEX_SHADER, "../shaders/" + name + ".vert");
		frag = Shader::CreateFromFile(GL_FRAGMENT_SHADER, "../shaders/" + name + ".frag");
		program = ShaderProgram::Create();
		program.AttachShader(vert);
		program.AttachShader(frag);
		program.Link();
		program.DetachShader(vert);
		program.DetachShader(frag);
		vert.Delete();
		frag.Delete();
	} catch (ShaderError e){
		std::cerr << e.what() << std::endl;
		exit(-1);
	} catch (ShaderProgramError e){
		std::cerr << e.what() << std::endl;
		exit(-1);
	}
	
	programs[name].reset(new ShaderProgram(std::move(program)));
	return programs[name];
}

ShaderProgram ShaderProgram::Create(){
	ShaderProgram program;
	program.id = glCreateProgram();
	if (program.id == 0){
		program.ThrowShaderProgramError("glCreateProgram returned 0");
	}
	return program;
}

void ShaderProgram::AttachShader(const Shader& shader){
	glAttachShader(id, shader.GetId());
}

void ShaderProgram::DetachShader(const Shader& shader){
	glDetachShader(id, shader.GetId());
}

void ShaderProgram::Link(){
	glLinkProgram(id);
	GLint isLinked;
	glGetProgramiv(id, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_TRUE){
		GetAllInputResourceProperties();
	} else{
		ThrowShaderProgramError("glLinkProgram failed");
	}
}

bool ShaderProgram::IsValid() const{
	glValidateProgram(id);
	GLint isValid;
	glGetProgramiv(id, GL_VALIDATE_STATUS, &isValid);
	return isValid == GL_TRUE;
}

std::string ShaderProgram::GetInfoLog() const{
	GLint logLength = GetLogLength();

	std::string log;

	if (id != 0){
		GLsizei logLength = GetLogLength();
		log.resize(logLength);
		glGetProgramInfoLog(id, logLength, &logLength, &log[0]);
	}

	return log;
}

GLint ShaderProgram::GetLogLength() const{
	GLint logLength = 0;

	if (id != 0){
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
	}

	return logLength;
}

void ShaderProgram::ThrowShaderProgramError(const std::string& reason){
	std::ostringstream error;

	if (id != 0){
		error << "Failed to create ShaderProgram: " << reason << ".\n"
			<< "Program Log:\n" << GetInfoLog();
	} else{
		error << "Failed to create shaderProgram: " << reason << ".";
	}

	throw ShaderProgramError(error.str());
}

void ShaderProgram::GetAllInputResourceProperties(){
	GetAllUniformNames();
	GetAllUniformLocations();
}

void ShaderProgram::GetAllUniformLocations(){
	for (const auto& uniform : uniforms){
		uniforms[uniform.first].location = GetResourceLocation(GL_UNIFORM, uniform.first.c_str());
	}
}

void ShaderProgram::GetAllUniformNames(){
	uniforms = GetAllResourceNames(GL_UNIFORM);
}

std::map<std::string, ShaderProgram::LocationValue> ShaderProgram::GetAllResourceNames(GLenum resource){
	std::map<std::string, LocationValue> resourceNames;

	GLint indexCount;
	GLint maxLength;
	glGetProgramInterfaceiv(id, resource, GL_ACTIVE_RESOURCES, &indexCount);
	glGetProgramInterfaceiv(id, resource, GL_MAX_NAME_LENGTH, &maxLength);

	std::string name;
	name.resize(maxLength);
	GLsizei nameLength;
	for (GLint i = 0; i < indexCount; ++i){
		glGetProgramResourceName(id, resource, i, maxLength, &nameLength, &name[0]);

		resourceNames[name.c_str()].location = -1;
	}

	return resourceNames;
}

ShaderProgram::LocationValue ShaderProgram::GetResourceLocation(GLenum resource, const std::string & name){
	LocationValue location;
	location.location = glGetProgramResourceLocation(id, resource, name.c_str());
	return location;
}

ShaderProgramError::ShaderProgramError(const std::string & message) : runtime_error(message){
}

const char * ShaderProgramError::what() const{
	return runtime_error::what();
}
