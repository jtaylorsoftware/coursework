/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

// Get rid of WinAPI macros
#ifdef ReadFile
#undef ReadFile
#endif

#include "glad/glad.h"

#include <string>

// 
//Useful to cast std::string to const char ** in one line
//IS NOT a native OpenGL type but is named to clarify its use.
//
struct GLString
{
	const char* str_;
	GLString(const std::string& str);
	operator const char **();
};

std::string ReadFile(const std::string& filePath);

std::string OpenGlEnumToString(GLenum glEnum);