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

// NOTE: PlaneTBN is temporary to just follow along with Learn OpenGL tutorial
//class PlaneTBN
//{
//private:
//
//	std::vector<VertexTBN> Vertices;
//	std::vector<unsigned int> Indices;
//
//	VertexArray m_VAO;
//	VertexBuffer m_VBO;
//	VertexBufferLayout m_Layout;
//	IndexBuffer m_EBO;
//
//	float m_UVTileScale = 1.0f;
//
//	void GenerateVertices();
//	void GenerateQuad();
//public:
//	PlaneTBN();
//	PlaneTBN(float uvTiling);
//	~PlaneTBN();
//
//	void Draw(Shader& shader, const Renderer& renderer);
//};

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