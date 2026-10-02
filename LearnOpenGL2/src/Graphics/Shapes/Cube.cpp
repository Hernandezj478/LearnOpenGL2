#include "Cube.h"

#include "Graphics/Renderer.h"
#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"

Cube::Cube()
{
	GenerateVertices();
}

Cube::~Cube() = default;

void Cube::GenerateVertices()
{
	m_Vertices.clear();
	m_Indices.clear();

	float m = 0.5f;
	// OpenGL expects CCW rotation for points/faces
	const glm::vec3 basePoints[8] =
	{
		{  m, -m,  m},	//P0	//0
		{ -m, -m,  m},	//P1	//1
		{ -m, -m, -m},	//P2	//2
		{  m, -m, -m},	//P3	//3

		{  m,  m, -m},	//P4	//4
		{ -m,  m, -m},	//P5	//5
		{ -m,  m,  m},	//P6	//6
		{  m,  m,  m}	//P7	//7
	};
	
	const unsigned int faceCorners[6][4] =
	{
		{0, 1, 2, 3},	// Bottom
		{4, 5, 6, 7},	// Top
		{6, 5, 2, 1},	// Left
		{4, 7, 0, 3},	// Right
		{7, 6, 1, 0},	// Front
		{5, 4, 3, 2},	// Back
	};
	
	const glm::vec3 faceNormals[6] = {
		{ 0.0, -1.0,  0.0},	//Bottom
		{ 0.0,  1.0,  0.0},	//Top
		{-1.0,  0.0,  0.0},	//Left
		{ 1.0,  0.0,  0.0},	//Right
		{ 0.0,  0.0,  1.0},	//Front
		{ 0.0,  0.0, -1.0},	//Back
	};

	const glm::vec2 texCoords[4] = {
		glm::vec2{0.0f, 0.0f}, // bottom-left
		glm::vec2{1.0f, 0.0f}, // bottom-right
		glm::vec2{1.0f, 1.0f}, // top-right
		glm::vec2{0.0f, 1.0f}  // top-left
	};

	std::unordered_map<Vertex, unsigned int> vertexMap;


	for (int face = 0; face < 6; face++)
	{
		const glm::vec3 faceNormal = faceNormals[face];

		const glm::vec3& p0 = basePoints[faceCorners[face][0]];
		const glm::vec3& p1 = basePoints[faceCorners[face][1]];
		const glm::vec3& p2 = basePoints[faceCorners[face][2]];
		const glm::vec3 edge1 = p1 - p0;
		const glm::vec3 edge2 = p2 - p0;

		const glm::vec2 deltaUV1 = texCoords[1] - texCoords[0];
		const glm::vec2 deltaUV2 = texCoords[2] - texCoords[0];

		const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		const glm::vec3 faceTangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
		const glm::vec3 faceBitangent = f * (-deltaUV2.x * edge1 + deltaUV1.y * edge2);


		const int triIndices[2][3] =
		{
			{0, 1, 2},
			{2, 3, 0}
		};

		const int texLookup[4] = { 0, 1, 2, 3 };
		for (int tri = 0; tri < 2; tri++)
		{
			for (int corner = 0; corner < 3; corner++)
			{
				const int cornerSlot = triIndices[tri][corner];
				const int pointIndx = faceCorners[face][cornerSlot];
				
				glm::vec3 pos = basePoints[pointIndx];
				glm::vec2 uv = texCoords[cornerSlot];
				glm::vec3 norm = faceNormal;
				
				Vertex v(pos, uv, norm, faceTangent, faceBitangent);
				if (vertexMap.count(v) == 0)
				{
					unsigned int index = static_cast<unsigned int>(m_Vertices.size());
					m_Vertices.push_back(v);
					vertexMap[v] = index;
				}

				m_Indices.push_back(vertexMap[v]);
			}
		}
	}

	m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
	m_Layout = std::make_unique<VertexBufferLayout>();

	m_Layout->Push<float>(3);	// position
	m_Layout->Push<float>(2);	// texture
	m_Layout->Push<float>(3);	// normal
	m_Layout->Push<float>(3);	// tangent
	m_Layout->Push<float>(3);	// bitangent
	
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_EBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
	m_VAO->Unbind();
	m_VBO->Unbind();
	m_EBO->Unbind();
}

void Cube::Draw(Shader& shader, const Renderer& renderer)
{
	renderer.Draw(*m_VAO, *m_EBO, shader);
	m_VAO->Unbind();
}

void Cube::DrawPoints(Shader& shader, const Renderer& renderer)
{
	renderer.DrawPoint(*m_VAO, shader, 8);
	m_VAO->Unbind();
}

void Cube::CreatePoints()
{
	m_Vertices.clear();
	m_Indices.clear();

	const glm::vec3 basePoints[8] =
	{
		{  0.5f, -0.5f,  0.5f},	//P0	//0
		{ -0.5f, -0.5f,  0.5f},	//P1	//1
		{ -0.5f, -0.5f, -0.5f},	//P2	//2
		{  0.5f, -0.5f, -0.5f},	//P3	//3
		{ -0.5f,  0.5f,  0.5f},	//P4	//4
		{  0.5f,  0.5f,  0.5f},	//P5	//5
		{  0.5f,  0.5f, -0.5f},	//P6	//6
		{ -0.5f,  0.5f, -0.5f}	//P7	//7
	};

	m_VBO = std::make_unique<VertexBuffer>(basePoints, 8 * sizeof(glm::vec3));
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(3);

	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_VAO->Unbind();
}

void Cube::PrintVertices()
{
	for (int i = 0; i < m_Vertices.size(); ++i)
	{
		std::cout << m_Vertices[i].GetDataAsString() << std::endl;
	}
}


