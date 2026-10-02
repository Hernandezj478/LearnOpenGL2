#pragma once
#include "Graphics/Vertex.h"
#include "Graphics/Shader.h"

#include <memory>
#include <vector>

constexpr int X_SEGMENTS = 64;
constexpr int Y_SEGMENTS = 64;
constexpr float PI = 3.14159265359;

class VertexArray;
class VertexBuffer;
class VertexBufferLayout;
class IndexBuffer;
class Renderer;


class Sphere
{
public:
	Sphere();
	~Sphere();

	void Draw(Shader& shader, const Renderer& renderer);

private:
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;


	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::unique_ptr<IndexBuffer> m_EBO;
};