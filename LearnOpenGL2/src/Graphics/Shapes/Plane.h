#pragma once

#include "Graphics/Vertex.h"
#include "Graphics/Shader.h"

#include <memory>
#include <vector>

class VertexArray;
class VertexBuffer;
class VertexBufferLayout;
class IndexBuffer;
class Shader;
class Renderer;

class Plane
{
public:
	Plane();
	~Plane();

	void Draw(Shader& shader, const Renderer& renderer);
	void DrawPoints(Shader& shader, const Renderer& renderer);

	void CreateInstance(std::vector<glm::mat4> transforms);
private:
	
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::unique_ptr<IndexBuffer> m_EBO;

	std::unique_ptr<VertexBuffer> m_InstanceBuffer;

	unsigned int m_InstanceCount = 1;

	void GenerateVertices();

};

class Plane2D
{
public:
	Plane2D();
	~Plane2D();

	void Draw(Shader& shader, const Renderer& renderer);
private:
	std::vector<Vertex2D> Vertices;
	std::vector<unsigned int> Indices;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::unique_ptr<IndexBuffer> m_EBO;

	void GenerateVertices();
};