#include "Glyph.h"

#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"

#include "Graphics/Renderer.h"

Glyph::Glyph()
{
	GenerateVertices();
}

Glyph::~Glyph() = default;

void Glyph::Draw(Shader & shader, const Renderer & renderer)
{
	renderer.Draw(*m_VAO, *m_EBO, shader, m_InstanceCount);
}

void Glyph::CreateInstance(std::vector<GlyphInstance> instances)
{
	m_InstanceCount = instances.size();

	m_InstancedBuffer = std::make_unique<VertexBuffer>(instances.data(), instances.size() * sizeof(GlyphInstance), GL_DYNAMIC_DRAW);
	m_VAO->Bind();
	VertexBufferLayout layout;
	layout.Push<glm::vec4>(1, 1);
	layout.Push<glm::mat4>(0, 1);

	m_VAO->AddInstancedBuffer(*m_InstancedBuffer, layout, sizeof(GlyphInstance));
}

void Glyph::GenerateVertices()
{
	const glm::vec2 basePoints[] =
	{
		{1.0f, 0.0f},	// Bottom-right
		{1.0f, 1.0f},	// Top-right
		{0.0f, 1.0f},	// Top-left
		{0.0f, 0.0f},	// Bottom-left
	};

	const glm::vec2 texCoords[] =
	{
		{1.0f, 0.0f},	// Bottom-right
		{1.0f, 1.0f},	// Top-right
		{0.0f, 1.0f},	// Top-left
		{0.0f, 0.0f}	// Bottom-left
	};

	Vertex vertex;
	for (int i = 0; i < 4; i++)
	{
		vertex.first = basePoints[i];
		vertex.second = texCoords[i];
		m_Vertices.push_back(vertex);
	}

	m_Indices.push_back(0u);
	m_Indices.push_back(1u);
	m_Indices.push_back(2u);

	m_Indices.push_back(2u);
	m_Indices.push_back(3u);
	m_Indices.push_back(0u);

	m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex), GL_STATIC_DRAW);
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(2);	// Position
	m_Layout->Push<float>(2);	// Texture

	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_EBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
	m_VAO->Unbind();
}
