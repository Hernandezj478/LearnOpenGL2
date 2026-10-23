#include "VertexBuffer.h"

#include <glad/glad.h>

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &m_RendererID);
}

VertexBuffer::VertexBuffer(const void* data, unsigned int sizeInBytes, int usage)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, usage);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Init(const void* data, unsigned int sizeInBytes)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, sizeInBytes, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::AddSubData(unsigned int offset, unsigned int size, void* data)
{
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}
