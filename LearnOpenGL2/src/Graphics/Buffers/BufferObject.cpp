#include "BufferObject.h"

BufferObject::BufferObject(unsigned int target)
{
	m_TargetBuffer = target;
	glGenBuffers(1, &m_RendererID);
}

BufferObject::~BufferObject()
{
	glDeleteBuffers(1, &m_RendererID);
}

void BufferObject::Bind() const
{
	glBindBuffer(m_TargetBuffer, m_RendererID);
}

void BufferObject::Unbind() const
{
	glBindBuffer(m_TargetBuffer, 0);
}

void BufferObject::CreateBufferObject(void* data, unsigned int byteSize)
{
	Bind();
	glBufferData(m_TargetBuffer, byteSize, data, GL_STATIC_DRAW);
}

void BufferObject::SubBufferData(unsigned int offset, unsigned int byteSize, void* data)
{
	Bind();
	glBufferSubData(m_TargetBuffer, offset, byteSize, data);
}
