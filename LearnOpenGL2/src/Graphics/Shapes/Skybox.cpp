#include "Skybox.h"

#include "Graphics/Renderer.h"
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"

Skybox::Skybox()
{
	GenerateVertices();
}

Skybox::~Skybox() = default;

void Skybox::Draw(Shader& shader, const Renderer& renderer)
{
	renderer.Draw(*m_VAO, *m_EBO, shader);
	m_VAO->Unbind();
}

void Skybox::GenerateVertices()
{
	m_Vertices.clear();
	m_Indices.clear();

	const glm::vec3 basePoints[8] =
	{
		{  1.0f, -1.0f, -1.0f},	//P0	//0
		{ -1.0f, -1.0f, -1.0f},	//P1	//1
		{ -1.0f, -1.0f,  1.0f},	//P2	//2
		{  1.0f, -1.0f,  1.0f},	//P3	//3

		{  1.0f,  1.0f,  1.0f},	//P4	//4
		{ -1.0f,  1.0f,  1.0f},	//P5	//5
		{ -1.0f,  1.0f, -1.0f},	//P6	//6
		{  1.0f,  1.0f, -1.0f}	//P7	//7
	};

	const unsigned int faceCorners[6][4] =
	{
		{0, 1, 2, 3},	// Bottom
		{4, 5, 6, 7},	// Top
		{2, 1, 6, 5},	// Left
		{0, 3, 4, 7},	// Right
		{3, 2, 5, 4},	// Front
		{1, 0, 7, 6},	// Back
	};

	std::unordered_map<glm::vec3, unsigned int> vertexMap;


	for (int face = 0; face < 6; face++)
	{
		const int triIndices[2][3] =
		{
			{0, 1, 2},
			{2, 3, 0}
		};

		for (int tri = 0; tri < 2; tri++)
		{
			for (int corner = 0; corner < 3; corner++)
			{
				int pointIndx = faceCorners[face][triIndices[tri][corner]];
				glm::vec3 pos = basePoints[pointIndx];

				if (vertexMap.count(pos) == 0)
				{
					unsigned int index = static_cast<unsigned int>(m_Vertices.size());
					m_Vertices.push_back(pos);
					vertexMap[pos] = index;
				}

				m_Indices.push_back(vertexMap[pos]);
			}
		}
	}

	m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(glm::vec3));
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(3);

	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_EBO = std::make_unique<IndexBuffer>();
	m_EBO->Init(m_Indices.data(), m_Indices.size());
	m_VAO->Unbind();
	m_VBO->Unbind();
	m_EBO->Unbind();
}
