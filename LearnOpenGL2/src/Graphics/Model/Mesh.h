#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Vertex.h"
#include "Graphics/Buffers/VertexArray.h"

#include <memory>
#include <vector>

class VertexBuffer;
class VertexBufferLayout;
class IndexBuffer;
class Renderer;
class Texture;

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures);
	~Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = default;
	Mesh& operator=(Mesh&&) = default;
	void SetupMesh();

	void Draw(Shader& shader, const Renderer& renderer, unsigned int instanceCount = 1);
	
	const std::vector<std::shared_ptr<Texture>>& GetTextures() const { return m_Textures; }

	VertexArray& GetVAO() { return *m_VAO; }
	const VertexArray& GetVAO() const { return *m_VAO; }
	unsigned int GetIndicesSize() { return m_Indices.size(); }

private:
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<IndexBuffer> m_EBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;

	std::vector<unsigned int> m_Indices;
	std::vector<Vertex> m_Vertices;
	std::vector<std::shared_ptr<Texture>> m_Textures;

};