/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#pragma once

#include <string>
#include <vector>

struct Vec2f
{
	float x;
	float y;
};

struct Vec3f
{
	float x;
	float y;
	float z;
};

struct Vertex
{
	Vec3f position;
	Vec3f normal;
	Vec3f tangent;
	Vec2f textureCoordinate;
};

struct Image
{
	unsigned int width;
	unsigned int height;
	std::vector<unsigned char> data;
};

// Types of textures used for shaders
enum class TextureType : short
{
	Diffuse = 0,
	Ambient = 1,
	Specular_Roughness = 2, // specular in blinn, roughness in PBR
	Metallic = 3, // metallic PBR
	Normal = 4,
	Displacement = 5,
	Emissive = 6,
	Height = 7,
	Unknown = 8
};

// Stores the path and type of image so it can be loaded later
struct RawTexture
{
	TextureType type;
	std::string path;
};

// Stores the vertex data loaded from a mesh file
struct RawMesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<RawTexture> textures;
};

// Stores all the mesh files that were found in some .model or other 3d model file
struct RawModel
{
	std::vector<RawMesh> meshes;
};
