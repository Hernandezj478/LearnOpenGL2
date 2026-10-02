#pragma once

#include "Graphics/Vertex.h"
#include "Graphics/Shader.h"

#include <memory>
#include <vector>

class Shader;
class Renderer;
class VertexArray;
class VertexBuffer;
class VertexBufferLayout;
class IndexBuffer;

class Skybox
{
public:
	Skybox();
	~Skybox();

	void Draw(Shader& shader, const Renderer& renderer);

private:
	std::vector<glm::vec3> m_Vertices;
	std::vector<unsigned int> m_Indices;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	std::unique_ptr<IndexBuffer> m_EBO;

	void GenerateVertices();
};