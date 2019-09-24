/*
Jeremy Taylor CS 114 Final Project Spring 2018
*/

#include "engine/texture_library.hpp"

#include "opengl/texture.hpp"
#include "opengl/texture2d.hpp"

#include "utility/image_utility.hpp"

std::map<std::string, std::shared_ptr<Texture>> TextureLibrary::textures;

std::shared_ptr<Texture> TextureLibrary::LoadTexture2d(std::string filePath){
	std::shared_ptr<Texture> texture = nullptr;

	if (textures.count(filePath) != 0){
		texture = textures[filePath];
	} else{
		Image image = ImageLoader::LoadImage(filePath, true);

		if (image.data != nullptr){
			texture.reset(new Texture2d());

			TextureStorageFormat storageFormat;
			TextureImage texImage;

			storageFormat.width = texImage.format.width = image.width;
			storageFormat.height = texImage.format.height = image.height;
			storageFormat.levels = 1;

			texImage.format.level = 0;
			texImage.format.offsetX = texImage.format.offsetY = 0;
			texImage.format.dataType = GL_UNSIGNED_BYTE;
			texImage.data = image.data;

			switch (image.numComponents){
			case 1:
				storageFormat.internalFormat = GL_R8;
				texImage.format.dataFormat = GL_RED;
				break;
			case 2:
				storageFormat.internalFormat = GL_RG8;
				texImage.format.dataFormat = GL_RG;
			case 3:
				storageFormat.internalFormat = GL_RGB8;
				texImage.format.dataFormat = GL_RGB;
				break;
			case 4:
				storageFormat.internalFormat = GL_RGBA8;
				texImage.format.dataFormat = GL_RGBA;
				break;
			}
			texture->AllocateStorage(storageFormat);
			texture->SetImage(texImage);
			texture->SetParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
			texture->SetParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
			texture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			texture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			textures[filePath] = texture;

			ImageLoader::FreeImage(image);
		}
	}
	return texture;
}

std::shared_ptr<Texture> TextureLibrary::LoadTexture2dSrgb(std::string filePath){
	std::shared_ptr<Texture> texture = nullptr;

	if (textures.count(filePath) != 0){
		texture = textures[filePath];
	} else{
		Image image = ImageLoader::LoadImage(filePath, true);

		if (image.data != nullptr){
			texture.reset(new Texture2d());

			TextureStorageFormat storageFormat;
			TextureImage texImage;

			storageFormat.width = texImage.format.width = image.width;
			storageFormat.height = texImage.format.height = image.height;
			storageFormat.levels = 1;

			texImage.format.level = 0;
			texImage.format.offsetX = texImage.format.offsetY = 0;
			texImage.format.dataType = GL_UNSIGNED_BYTE;
			texImage.data = image.data;

			switch (image.numComponents){
			case 1:
				storageFormat.internalFormat = GL_R8;
				texImage.format.dataFormat = GL_RED;
				break;
			case 2:
				storageFormat.internalFormat = GL_RG8;
				texImage.format.dataFormat = GL_RG;
			case 3:
				storageFormat.internalFormat = GL_SRGB8;
				texImage.format.dataFormat = GL_RGB;
				break;
			case 4:
				storageFormat.internalFormat = GL_SRGB8_ALPHA8;
				texImage.format.dataFormat = GL_RGBA;
				break;
			}

			texture->AllocateStorage(storageFormat);
			texture->SetImage(texImage);
			texture->SetParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
			texture->SetParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
			texture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			texture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			textures[filePath] = texture;

			ImageLoader::FreeImage(image);
		}
	}
	return texture;
}

std::shared_ptr<Texture> TextureLibrary::LoadHdrTexture(std::string filePath){
	std::shared_ptr<Texture> texture = nullptr;

	if (textures.count(filePath) != 0){
		texture = textures[filePath];
	} else{
		Image image = ImageLoader::LoadImage(filePath, true);

		if (image.data != nullptr){
			texture.reset(new Texture2d());

			TextureStorageFormat storageFormat;
			TextureImage texImage;

			storageFormat.width = texImage.format.width = image.width;
			storageFormat.height = texImage.format.height = image.height;
			storageFormat.levels = 1;

			texImage.format.level = 0;
			texImage.format.offsetX = texImage.format.offsetY = 0;
			texImage.format.dataType = GL_FLOAT;
			texImage.data = image.data;
			
			storageFormat.internalFormat = GL_RGB16F;
			texImage.format.dataFormat = GL_RGB;

			texture->AllocateStorage(storageFormat);
			texture->SetImage(texImage);
			texture->SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			texture->SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			texture->SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			texture->SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			textures[filePath] = texture;

			ImageLoader::FreeImage(image);
		}
	}
	return texture;
}
