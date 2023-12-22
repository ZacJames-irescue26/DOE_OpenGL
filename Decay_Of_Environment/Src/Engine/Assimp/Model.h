#pragma once

#include "../config.h"

#include "mesh.h"
#include "Engine/shader.h"
#include "Engine/Material.h"

class AssimpGLMHelpers
{
public:

	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};
#define MAX_NUM_BONES_PER_VERTEX 16
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(uint32_t))
struct VertexBoneData
{
	uint32_t BoneIDs[MAX_NUM_BONES_PER_VERTEX] = {0};
	float Weights[MAX_NUM_BONES_PER_VERTEX] = {0.0f};

	VertexBoneData()
	{

	}
	void AddBoneData(uint32_t BoneID, float Weight)
	{
		//std::cout << "Bone Array size: " << ARRAY_SIZE_IN_ELEMENTS(BoneIDs) << std::endl;
		for (uint32_t i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); i++)
		{
			if (Weights[i] == 0)
			{
				BoneIDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}
		assert(0);
	}
};

//struct BoneInfo
//{
//	glm::mat4 OffsetMatrix;
//	glm::mat4 FinalTransformation;
//
//	BoneInfo(const glm::mat4& Offset)
//	{
//		OffsetMatrix = Offset;
//		FinalTransformation;
//	}
//};
struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;

	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset;

};
class Model
{
public:
	//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

	// model data 
	std::shared_ptr<Material> Modelmaterial;
	std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh>    meshes;
	std::vector<VertexBoneData> vertex_To_Bones;
	std::vector<int> mesh_base_vertex;
	std::map<std::string, uint32_t> BoneNameToIndexMap;
	std::vector<BoneInfo> m_BoneInfo;
	const aiScene* scene = nullptr;
	glm::mat4 m_GlobalInverseTransform;
	int total_vertices;
	Assimp::Importer importer;
	std::string directory;
	std::string m_path;
	int EntityId = -1;
	bool gammaCorrection;
	bool isLamp;
	float scale;

	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;
	auto& GetBoneInfoMap() { return m_BoneInfoMap; }
	int& GetBoneCount() { return m_BoneCounter; }
	
	const std::string& GetPath() {return m_path;}
	void SetPath(std::string path) {m_path.assign(path); }


	// constructor, expects a filepath to a 3D model.
	Model(std::string const& path, int entityID = -1, bool gamma = false)
		: gammaCorrection(gamma), EntityId(entityID)
	{
		this->isLamp = isLamp;
		this->scale = scale;
		SetPath(path);
		Modelmaterial = std::make_shared<Material>();
		loadModel(path);
	}
	Model() {}
	/*void RecalculateModel()
	{
		Modelmaterial.reset();

	}*/
	// draws the model, and thus all its meshes
	void Draw(Shader& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}


private:



	

	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(std::string const& path)
	{
		// read file via ASSIMP
		
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		
		if (scene) {
			m_GlobalInverseTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation);
			m_GlobalInverseTransform = glm::inverse(m_GlobalInverseTransform);
		}
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
			return;
		}
		// retrieve the directory path of the filepath
		if (scene->GetEmbeddedTexture(path.c_str()) == nullptr)
		{
			Modelmaterial->directory = path.substr(0, path.find_last_of('\\'));
			if (Modelmaterial->directory == m_path)
			{
				Modelmaterial->directory = path.substr(0, path.find_last_of('/'));

			}
		}
		else
		{
			const aiTexture* texture = scene->GetEmbeddedTexture(path.c_str());

			Modelmaterial->LoadEmbeddedTexture(texture, texture->mFilename.C_Str());
		}


		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene)
	{
		
		// process each mesh located at the current node
		//assert(node->mNumMeshes != 0 && "Number of meshes is 0: %i", node->mNumMeshes);
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			meshes.push_back(processMesh(mesh, scene, i));
			
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene, int mesh_index)
	{
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}
			vertex.EntityId = EntityId;
			if (mesh->mTangents)
			{
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
			}
			else
			{
				vertex.Tangent = glm::vec3(1.0f);
			}
			if (mesh->mBitangents)
			{
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}
			else
			{
				vertex.Bitangent = glm::vec3(1.0f);
			}
			
			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		Modelmaterial->loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
		// 2. specular maps
		Modelmaterial->loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::Specular);

		// 3. normal maps
		Modelmaterial->loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::Normal);

		// 4. height maps
		Modelmaterial->loadMaterialTextures(material, aiTextureType_AMBIENT, TextureType::Height);
		// 5.0 metalness
		Modelmaterial->loadMaterialTextures(material, aiTextureType_METALNESS, TextureType::Metalness);
		
		
		if (mesh->HasBones())
		{
			ExtractBoneWeightForVertices(vertices, mesh, scene);
		}
		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, Modelmaterial, isLamp, scale);
	}

	void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			if (vertex.m_BoneIDs[i] < 0)
			{
				vertex.m_Weights[i] = weight;
				vertex.m_BoneIDs[i] = boneID;
				break;
			}
		}
	}


	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
	{
		auto& boneInfoMap = m_BoneInfoMap;
		int& boneCount = m_BoneCounter;

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}
			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}
	
	
};


