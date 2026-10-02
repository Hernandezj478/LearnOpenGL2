#pragma once

#include "Mesh.h"
#include "Common/Enums.h"
#include "Graphics/Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>
#include <string>
#include <vector>

class VertexBuffer;
class VertexBufferLayout;
class Texture;

class Model
{
public:
	Model(std::string filepath);
	~Model();
	void Draw(Shader& shader,const Renderer& renderer);
	void Finalize();

	unsigned int GetNumberOfMeshes() const { return m_Meshes.size(); }
	void BindMeshAtIndex(unsigned int index);
	unsigned int GetMeshIndexSize(unsigned int index);
	void CreateInstance(std::vector<glm::mat4> modelMatrices);
	void IntanceData(unsigned int index, const VertexBufferLayout& layout);

	bool operator==(const Model& other)
	{
		return m_FilePath == other.m_FilePath;
	}

	std::string m_FilePath;
private:
	std::vector<Mesh> m_Meshes;

	std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform);
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* material, aiTextureType type, const TextureType& typeName);

	std::string NormalizePath(const std::string& path);
	std::string GetFileName(const std::string& path);
	std::unique_ptr<VertexBuffer> m_InstanceBuffer;
	unsigned int m_InstanceCount = 1;
};