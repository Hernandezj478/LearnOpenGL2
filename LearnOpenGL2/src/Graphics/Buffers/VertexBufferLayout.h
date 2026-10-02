#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <stdexcept>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
	unsigned int divisor;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT: return sizeof(float);
		case GL_UNSIGNED_INT: return sizeof(unsigned int);
		case GL_UNSIGNED_BYTE: return sizeof(unsigned char);
		default:
			//Does nothing
			return 0;
		}
	}
};

class VertexBufferLayout
{
private:
	unsigned int m_Stride;
	std::vector<VertexBufferElement> m_Elements;
public:
	VertexBufferLayout() : m_Stride(0) {};
	~VertexBufferLayout() = default;

	template<typename T>
	void Push(unsigned int count, unsigned int divisor = 0)
	{
		std::runtime_error(false);
	}

	template<>
	void Push<unsigned char>(unsigned int count, unsigned int divisor)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE, divisor });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_BYTE);
	}

	template<>
	void Push<unsigned int>(unsigned int count, unsigned int divisor)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE, divisor });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<float>(unsigned int count, unsigned int divisor)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE, divisor });
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<glm::mat4>(unsigned int count, unsigned int divisor) 
	{
		for (int i = 0; i < 4; i++) 
		{
			m_Elements.push_back({ GL_FLOAT, 4, GL_FALSE, divisor });
		}
		m_Stride += sizeof(glm::mat4);
	}

	inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};