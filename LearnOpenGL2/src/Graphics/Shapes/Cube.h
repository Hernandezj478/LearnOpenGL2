#pragma once

#include "Graphics/Vertex.h"
#include "Graphics/Shader.h"

#include <memory>
#include <vector>

class Renderer;
class VertexArray;
class VertexBuffer;
class VertexBufferLayout;
class IndexBuffer;

class Cube
{
public:
	Cube();
	~Cube();

	void Draw(Shader& shader, const Renderer& renderer);
	void DrawPoints(Shader& shader, const Renderer& renderer);

	void CreatePoints();

	void PrintVertices();
private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::unique_ptr<IndexBuffer> m_EBO;

	void GenerateVertices();
};