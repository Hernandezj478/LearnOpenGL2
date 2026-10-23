#pragma once

#include <memory>
#include <vector>
#include <utility>

#include <glm/glm.hpp>

class VertexArray;
class VertexBuffer;
class VertexBufferLayout;
class IndexBuffer;
class Shader;
class Renderer;

using Vertex = std::pair<glm::vec2, glm::vec2>;
struct GlyphInstance
{
	glm::vec4 UVData;
	glm::mat4 transform;
};

class Glyph
{
public:
	Glyph();
	~Glyph();
	
	void Draw(Shader& shader, const Renderer& renderer);

	void CreateInstance(std::vector<GlyphInstance> instances);
private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<IndexBuffer> m_EBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;

	std::unique_ptr<VertexBuffer> m_InstancedBuffer;

	unsigned int m_InstanceCount = 1;

	void GenerateVertices();
};