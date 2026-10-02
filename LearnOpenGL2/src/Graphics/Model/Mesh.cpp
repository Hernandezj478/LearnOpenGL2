#include "Mesh.h"
#include "Common/Enums.h"

#include "Graphics/Texture.h"
#include "Graphics/Renderer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures) :
	m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{
}

Mesh::~Mesh() = default;

void Mesh::SetupMesh()
{
	m_VAO = std::make_unique<VertexArray>();
	m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(3); // Position
	m_Layout->Push<float>(2); // Texture
	m_Layout->Push<float>(3); // Normal
	m_Layout->Push<float>(3); // Tangent
	m_Layout->Push<float>(3); // Bitangent

	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_EBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
	m_VAO->Unbind();
	m_VBO->Unbind();
	m_EBO->Unbind();
}

void Mesh::Draw(Shader& shader, const Renderer& renderer, unsigned int instanceCount)
{
	shader.SetUniform1i("textureCount", static_cast<int>(m_Textures.size()));
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		m_Textures[i]->Bind(i);
		shader.SetUniform1i("textures[" + std::to_string(i) + "]", i);
		shader.SetUniform1i("textureTypes[" + std::to_string(i) + "]", (int)m_Textures[i]->GetType());
	}
	renderer.Draw(*m_VAO, *m_EBO, shader, instanceCount);
	m_VAO->Unbind();
}