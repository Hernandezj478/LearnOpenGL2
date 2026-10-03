#include "Model.h"

#include <sstream>

#include "Graphics/Texture.h"
#include "Graphics/Renderer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"

static glm::mat4 AssimpToGlm(const aiMatrix4x4& m)
{
	return glm::mat4(
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4);
}

Model::Model(std::string filepath)
{
	loadModel(filepath);
}

Model::~Model() = default;

void Model::Draw(Shader& shader, const Renderer& renderer)
{
	bool useInstance = (m_InstanceCount > 1) ? true : false;
	shader.Bind();
	shader.SetUniform1i("bUseInstanceMatrix", useInstance);
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].Draw(shader, renderer, m_InstanceCount);
	}
}

void Model::Finalize()
{
	for (Mesh& mesh : m_Meshes)
	{
		mesh.SetupMesh();
		for (const std::shared_ptr<Texture>& texture : mesh.GetTextures())
		{
			if (!texture->IsUploaded())
			{
				texture->Upload();
			}
		}
	}
}

void Model::BindMeshAtIndex(unsigned int index)
{
	m_Meshes[index].GetVAO().Bind();
}

unsigned int Model::GetMeshIndexSize(unsigned int index)
{
	return m_Meshes[index].GetIndicesSize();
}

void Model::CreateInstance(std::vector<glm::mat4> modelMatrices)
{
	m_InstanceCount = modelMatrices.size();
	m_InstanceBuffer = std::make_unique<VertexBuffer>(modelMatrices.data(), modelMatrices.size() * sizeof(glm::mat4));
	for (unsigned int i = 0; i < GetNumberOfMeshes(); i++)
	{
		BindMeshAtIndex(i);
		VertexBufferLayout layout;
		layout.Push<glm::mat4>(0, 1);	// Count must be added for this template to work correctly
		IntanceData(i, layout);
	}
}

void Model::IntanceData(unsigned int index, const VertexBufferLayout& layout)
{
	m_Meshes[index].GetVAO().AddInstancedBuffer(*m_InstanceBuffer, layout, sizeof(glm::mat4));
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::stringstream msg;
		msg << "[ERROR][ASSIMP] - " << importer.GetErrorString() << std::endl;
		throw std::runtime_error(msg.str());
	}

	m_FilePath = path.substr(0, path.find_last_of('/'));
	m_FilePath += "/";
	processNode(scene->mRootNode, scene, glm::mat4(1.0));
}

void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform)
{
	const glm::mat4 nodeTransform = parentTransform * AssimpToGlm(node->mTransformation);
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_Meshes.push_back(processMesh(mesh, scene, nodeTransform));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, nodeTransform);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& nodeTransform)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>> textures;

	const glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(nodeTransform)));

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector(1.0);
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = glm::vec3(nodeTransform * glm::vec4(vector, 1.0f));
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec(0.0);
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoord = vec;
			if (mesh->mTangents)
			{
				vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
				vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			}
		}
		else
		{
			vertex.TexCoord = glm::vec2(0.0f, 0.0f);
		}
		if (mesh->HasNormals())
		{
			vector = glm::vec3(1.0);
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = glm::normalize( normalMatrix * vector);
		}
		if (mesh->HasTangentsAndBitangents())
		{
			vector = glm::vec3(1.0);
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			vector = glm::vec3(1.0);
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}


		vertices.push_back(vertex);

	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// Basecolor/Diffuse Maps
	std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_BASE_COLOR, DIFFUSE);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// Fallback to legacy diffuse type
	if (textures.empty())
	{
		diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	// Specular Maps
	std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	// Normal Maps
	std::vector<std::shared_ptr<Texture>> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, NORMAL);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	// Emission Maps
	std::vector<std::shared_ptr<Texture>> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, EMISSIVE);
	textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());

	// Height Maps
	std::vector<std::shared_ptr<Texture>> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, HEIGHT);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// Ambient
	std::vector<std::shared_ptr<Texture>> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, AMBIENT);
	textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());

	// Ambient Occlusion * NOTE: Might need to remove since AO is multiplied to diffuse
	std::vector<std::shared_ptr<Texture>> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, AO);
	textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());

	// Roughtness Maps
	std::vector<std::shared_ptr<Texture>> roughness = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, ROUGHNESS);
	textures.insert(textures.end(), roughness.begin(), roughness.end());

	return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, const TextureType& typeName)
{
	std::vector<std::shared_ptr<Texture>> textures;
	std::string directory;

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);

		std::string name = GetFileName(str.C_Str());
		std::string path = m_FilePath + "textures/" + name;
		if (textureCache.find(path) != textureCache.end())
		{
			textures.push_back(textureCache[path]);
		}
		else
		{
			std::shared_ptr<Texture> texture = std::make_shared<Texture>(path);
			texture->SetType(typeName);
			texture->LoadPixels();
			textureCache[path] = texture;
			textures.push_back(textureCache[path]);
		}
	}

	return textures;
}

std::string Model::NormalizePath(const std::string& path)
{
	std::string fixed = path;
	std::replace(fixed.begin(), fixed.end(), '\\', '/');
	return fixed;
}

std::string Model::GetFileName(const std::string& path)
{
	std::string filepath = NormalizePath(path);
	return std::string(filepath.substr(filepath.find_last_of('/') + 1, filepath.size() - 1));
}
