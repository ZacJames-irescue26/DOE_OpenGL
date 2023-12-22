#pragma once
#include "Engine/config.h"
enum TextureType
{
	Diffuse,
	Normal,
	Specular,
	Height,
	Roughness,
	Metalness
};



class Material
{
public:
	Material();
	struct TextureData
	{
		unsigned int id;
		std::string path;
		bool operator< (const TextureData& lhs) const
		{
			return std::tie(id, path) < std::tie(lhs.id, lhs.path);
		}
	};
	void loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName);
	unsigned int LoadEmbeddedTexture(const aiTexture* texture, const char* FilePath);
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
	std::string TextureTypeToString(TextureType type);
	std::map<TextureData,TextureType> TextureDataToType;
	std::vector<TextureData> textures_loaded;
	std::string directory;
};