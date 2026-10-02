#include "VertexArray.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout)
{
	Bind();
	vertexBuffer.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void*)offset);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
	m_AttribIndexOffset = elements.size();
}

void VertexArray::AddInstancedBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout, unsigned int stride)
{
	Bind();
	vertexBuffer.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		unsigned int location = i + m_AttribIndexOffset;

		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, element.count, element.type, element.normalized, stride, (void*)offset);
		offset += sizeof(glm::vec4);
		
		glVertexAttribDivisor(location, element.divisor);
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}
