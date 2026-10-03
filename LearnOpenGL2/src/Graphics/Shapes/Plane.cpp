#include "Plane.h"

#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"

#include "Graphics/Renderer.h"

#include <unordered_map>

Plane::Plane()
{
	GenerateVertices();
}

Plane::~Plane() = default;

void Plane::GenerateVertices()
{
	const glm::vec3 basePoints[] =
	{
		{ 1.0f, 0.0f,  1.0f},
		{-1.0f, 0.0f,  1.0f},
		{-1.0f, 0.0f, -1.0f},
		{ 1.0f, 0.0f, -1.0f}
	};

	const glm::vec3 faceNormals ={0.0f, -1.0f, 0.0f,};

	const glm::vec2 texCoords[] =
	{
		{1.0f, 1.0f},
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f}
	};

	const int triIndices[2][3] =
	{
		{0, 1, 2},
		{2, 3, 0}
	};

	const glm::vec3 edge1 = basePoints[1] - basePoints[0];
	const glm::vec3 edge2 = basePoints[3] - basePoints[0];
	const glm::vec2 deltaUV1 = texCoords[1] - texCoords[0];
	const glm::vec2 deltaUV2 = texCoords[3] - texCoords[0];

	const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	const glm::vec3 tangent		= f * ( deltaUV2.y * edge1 - deltaUV1.y * edge2);
	const glm::vec3 bitangent	= f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

	Vertex vertex;
	for (int i = 0; i < 4; i++)
	{
		vertex.Position = basePoints[i];
		vertex.TexCoord = texCoords[i];
		vertex.Normal = faceNormals;
		vertex.Tangent = tangent;
		vertex.Bitangent = bitangent;
		Vertices.push_back(vertex);
	}

	for (int tri = 0; tri < 2; ++tri)
	{
		int i0 = triIndices[tri][0];
		int i1 = triIndices[tri][1];
		int i2 = triIndices[tri][2];

		Indices.push_back(i0);
		Indices.push_back(i1);
		Indices.push_back(i2);
	}

	m_VBO = std::make_unique<VertexBuffer>(Vertices.data(), Vertices.size() * sizeof(Vertex));
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(3);	// Position
	m_Layout->Push<float>(2);	// TexCoord
	m_Layout->Push<float>(3);	// Normal
	m_Layout->Push<float>(3);	// Tangent
	m_Layout->Push<float>(3);	// Bitangent

	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_EBO = std::make_unique<IndexBuffer>(Indices.data(), Indices.size());
	m_VAO->Unbind();
}

void Plane::Draw(Shader& shader, const Renderer& renderer)
{
	renderer.Draw(*m_VAO, *m_EBO, shader, m_InstanceCount);
	m_VAO->Unbind();
}

void Plane::DrawPoints(Shader& shader, const Renderer& renderer)
{
	renderer.DrawPoint(*m_VAO, shader, 4);
	m_VAO->Unbind();
}

void Plane::CreateInstance(std::vector<glm::mat4> transforms)
{
	m_InstanceCount = transforms.size();
	m_InstanceBuffer = std::make_unique<VertexBuffer>(transforms.data(), transforms.size() * sizeof(glm::mat4));
	m_VAO->Bind();
	VertexBufferLayout layout;
	layout.Push<glm::mat4>(0, 1);
	m_VAO->AddInstancedBuffer(*m_InstanceBuffer, layout, sizeof(glm::mat4));
}


Plane2D::Plane2D()
{
	GenerateVertices();
}

Plane2D::~Plane2D() = default;

void Plane2D::GenerateVertices()
{
	glm::vec2 basePoints[] =
	{
		{-1.0f, -1.0f},
		{ 1.0f, -1.0f},
		{ 1.0f,  1.0f},
		{-1.0f,  1.0f}
	};
	glm::vec2 texCoords[] =
	{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f},
	};

	unsigned int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};
	
	for (int i = 0; i < 4; i++)
	{
		Vertex2D vertex;
		vertex.Position = basePoints[i];
		vertex.TexCoord = texCoords[i];
		Vertices.push_back(vertex);
	}

	for (int i = 0; i < 6; i++)
	{
		Indices.push_back(indices[i]);
	}
	
	m_VBO = std::make_unique<VertexBuffer>(Vertices.data(), Vertices.size() * sizeof(Vertex2D));
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(2);	// Position
	m_Layout->Push<float>(2);	// TexCoord

	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_EBO = std::make_unique<IndexBuffer>(Indices.data(), Indices.size());
	m_VAO->Unbind();

}

void Plane2D::Draw(Shader& shader, const Renderer& renderer)
{
	renderer.Draw(*m_VAO, *m_EBO, shader);
	m_VAO->Unbind();
}