#include "Grid.h"

#include "Graphics/Renderer.h"

#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"

Grid::Grid(int gridSize) : m_GridSize(gridSize / 2)
{
	SetupGrid();
}

Grid::~Grid() = default;

void Grid::SetupGrid()
{
	for (int i = -m_GridSize; i <= m_GridSize; ++i)
	{
		float x = float(i);
		float z = float(i);

		m_GridLines.push_back(Line{
			glm::vec3(-float(m_GridSize), 0.0f, z),
			glm::vec3(float(m_GridSize), 0.0f, z)
		});

		m_GridLines.push_back(Line{
			glm::vec3{x, 0.0f, -float(m_GridSize)},
			glm::vec3{x, 0.0f, float(m_GridSize)}
		});
	}

	m_VBO = std::make_unique<VertexBuffer>(m_GridLines.data(), m_GridLines.size() * sizeof(Line));
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(3);
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_VAO->Unbind();
}

void Grid::Draw(Shader& shader, const Renderer& renderer) 
{
	shader.Bind();
	renderer.DrawLine(*m_VAO, shader, m_GridLines.size() * 2);
}