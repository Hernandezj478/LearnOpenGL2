#pragma once
#include <vector>
#include <memory>

#include "Graphics/Vertex.h"
#include "Graphics/Shader.h"

class VertexArray;
class VertexBuffer;
class VertexBufferLayout;
class Renderer;

class Grid
{
private:
	int m_GridSize = 0;
	std::vector<Line> m_GridLines;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
public:
	Grid(int gridSize);
	~Grid();
	void SetupGrid();
	void Draw(Shader& shader, const Renderer& renderer);
};