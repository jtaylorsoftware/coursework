/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

class Texture;

#include <map>
#include <memory>

// Static class that stores loaded images as Texture objects so that they only
// need to be loaded once in the lifetime of the program
class TextureLibrary
{
public:
	static std::shared_ptr<Texture> LoadTexture2d(std::string filePath);
	static std::shared_ptr<Texture> LoadTexture2dSrgb(std::string filePath);
	static std::shared_ptr<Texture> LoadHdrTexture(std::string filePath);
private:
	static std::map<std::string, std::shared_ptr<Texture>> textures;
};