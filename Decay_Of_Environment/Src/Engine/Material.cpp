#include "Material.h"


Material::Material()
{

}

void Material::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName)
{
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		//check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				TextureDataToType.emplace(textures_loaded[j], typeName);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			TextureData texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.path = str.C_Str();
			TextureDataToType.emplace(texture, typeName);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
}

unsigned int Material::LoadEmbeddedTexture(const aiTexture* texture, const char* FilePath)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image_data = nullptr;
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	if (texture->mHeight == 0)
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &nrComponents, 0);
	}
	else
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &nrComponents, 0);
	}
	if (image_data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(image_data);
	}
	else
	{
		std::cout << "Texture failed to load embedded: " << std::endl;
		stbi_image_free(image_data);
	}
	TextureData data;
	data.id = textureID;
	data.path = FilePath;
	std::cout << data.path << std::endl;
	//TextureDataToType
	return textureID;
}

unsigned int Material::TextureFromFile(const char* path, const std::string& directory, bool gamma /* = false */)
{
	stbi_set_flip_vertically_on_load(true);
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

std::string Material::TextureTypeToString(TextureType type)
{
	switch (type)
	{
	case Diffuse:
		return "Diffuse";
	case Normal:
		return "Normal";
	case Specular:
		return "Specular";
	case Height:
		return "Height";
	case Roughness:
		return "Roughness";
	case Metalness:
		return "Metalness";
	default:
	{
		std::cout << "Unknown type" << std::endl;
		return {};
	}
	}
}
