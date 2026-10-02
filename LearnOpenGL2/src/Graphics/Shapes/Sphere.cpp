#include "Sphere.h"

#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"

#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"
#include "Graphics/Buffers/IndexBuffer.h"

Sphere::Sphere()
{
	for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
	{
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			float xSegments = (float)x / (float)X_SEGMENTS;
			float ySegments = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegments * 2.0f * PI) * std::sin(ySegments * PI);
			float yPos = std::cos(ySegments * PI);
			float zPos = std::sin(xSegments * 2.0f * PI) * std::sin(ySegments * PI);

			glm::vec3 pos(xPos, yPos, zPos);
			glm::vec2 uv(xSegments, ySegments);
			glm::vec3 normal(xPos, yPos, zPos);

			Vertex v(pos, uv, normal);
			Vertices.push_back(v);
		}
	}
	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2, etc.
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				Indices.push_back(y * (X_SEGMENTS + 1) + x);
				Indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				Indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				Indices.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	m_VBO = std::make_unique<VertexBuffer>(Vertices.data(), Vertices.size() * sizeof(Vertex));
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(3);	// Position
	m_Layout->Push<float>(2);	// Texture
	m_Layout->Push<float>(3);	// Normal 
	m_Layout->Push<float>(3);	// Tangent 
	m_Layout->Push<float>(3);	// Bitangent 

	m_VAO = std::make_unique<VertexArray>();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);
	m_EBO = std::make_unique<IndexBuffer>(Indices.data(), Indices.size());
	m_VAO->Unbind();
}

Sphere::~Sphere() = default;

void Sphere::Draw(Shader& shader, const Renderer& renderer)
{
	m_VAO->Bind();
	renderer.Draw(*m_VAO, *m_EBO, shader, 1, GL_TRIANGLE_STRIP);
}