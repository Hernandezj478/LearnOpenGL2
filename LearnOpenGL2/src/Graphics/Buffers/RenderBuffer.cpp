#include "RenderBuffer.h"

RenderBuffer::RenderBuffer()
{
	glGenRenderbuffers(1, &m_RendererID);
}

RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &m_RendererID);
}

void RenderBuffer::Bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
}

void RenderBuffer::Unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::CreateStorage(GLint internalFormat, int screenWidth, int screenHeight)
{
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, screenWidth, screenHeight);
}

void RenderBuffer::CreateStorageMultiSample(GLenum internalFormat, int screenWidth, int screenHeight, int samples)
{
	Bind();
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, screenWidth, screenHeight);
}
