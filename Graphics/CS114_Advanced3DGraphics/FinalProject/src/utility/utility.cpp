/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "utility/utility.hpp"

#include <fstream>

GLString::GLString(const std::string& str) : str_(str.c_str()){}
GLString::operator const char **(){ return &str_; }

std::string ReadFile(const std::string& filePath){
	std::ifstream in(filePath.c_str(), std::ios::in | std::ios::binary);
	if (in){
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(static_cast<size_t>(in.tellg()));
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	std::string error = "ReadFile error: can't open file ";
	error.append(filePath.c_str());
	throw std::invalid_argument(error);
}

std::string OpenGlEnumToString(GLenum glEnum){
	const char* result;
	switch (glEnum){
	case GL_VERTEX_SHADER:
		result = "GL_VERTEX_SHADER";
		break;
	case GL_FRAGMENT_SHADER:
		result = "GL_FRAGMENT_SHADER";
		break;
	default:
		result = "";
		break;
	}
	return std::string(result);
}
