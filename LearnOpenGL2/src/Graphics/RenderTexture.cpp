#include "RenderTexture.h"
#include <stdexcept>

RenderTexture::RenderTexture(unsigned int bufferCount) : m_BufferCount(bufferCount), m_Width(0), m_Height(0)
{
	m_RendererID = new unsigned int[bufferCount];
	glGenTextures(m_BufferCount, m_RendererID);
	m_Target = TextureTarget::Texture2D;
}

RenderTexture::~RenderTexture()
{
	delete m_RendererID;
}

void RenderTexture::CreateColorBufferMSAA(int screenWidth, int screenHeight, int samples, GLenum internalFormat)
{
	m_Target = TextureTarget::Texture2DMS;
	for (unsigned int i = 0; i < m_BufferCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID[i]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, screenWidth, screenHeight, GL_TRUE);
	}
	Unbind();
}

void RenderTexture::CreateColorbufferHDR(int screenWidth, int screenHeight, GLint internalFormat, WrapType uvWrap, GLenum filter)
{
	m_Width = screenWidth;
	m_Height = screenHeight;
	m_Target = TextureTarget::Texture2D;
	for (unsigned int i = 0; i < m_BufferCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uvWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, uvWrap);
	}
}

void RenderTexture::CreateLUTBuffer(int width, int height, GLint internalFormat, GLenum format, GLenum type)
{
	m_Width = width;
	m_Height = height;
	m_Target = TextureTarget::Texture2D;

	glBindTexture(GL_TEXTURE_2D, m_RendererID[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void RenderTexture::CreateColorbuffer(int screenWidth, int screenHeight, GLenum internalFormat)
{
	m_Width = screenWidth;
	m_Height = screenHeight;

	m_Target = TextureTarget::Texture2D;

	for (unsigned int i = 0; i < m_BufferCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, internalFormat, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void RenderTexture::CreateDepthbuffer(int screenWidth, int screenHeight)
{
	m_Width = screenWidth;
	m_Height = screenHeight;
	for (unsigned int i = 0; i < m_BufferCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void RenderTexture::CreateDepthbufferTexture(int screenWidth, int screenHeight, WrapType uvWrap, GLenum filter)
{
	m_Width = screenWidth;
	m_Height = screenHeight;
	for (unsigned int i = 0; i < m_BufferCount; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uvWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, uvWrap);
	}
}

void RenderTexture::CreateStencilbuffer(int screenWidth, int screenHeight)
{
	m_Width = screenWidth;
	m_Height = screenHeight;
	for (unsigned int i = 0; i < m_BufferCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);


		glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, m_Width, m_Height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_INT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

void RenderTexture::CreteDepthStencilbuffer(int screenWidth, int screenHeight)
{
	m_Width = screenWidth;
	m_Height = screenHeight;

	for (unsigned int i = 0; i < m_BufferCount; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
			m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}
}

void RenderTexture::CreateBorder(glm::vec4 color)
{
	Bind();
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
}

void RenderTexture::Bind(unsigned int slot, unsigned int index) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	// Lets make sure the index is in bounds of the array
	if (index < m_BufferCount)
	{
		glBindTexture(GetTarget(), m_RendererID[index]);
		return;
	}
	throw std::runtime_error("Array out of bounds");
}

void RenderTexture::Unbind() const
{
	glBindTexture(GetTarget(), 0);
}

void RenderTexture::SetTarget(TextureTarget target)
{
	m_Target = target;
}

unsigned int RenderTexture::GetRendererID(unsigned int index) const
{
	if (index < m_BufferCount)
	{
		return m_RendererID[index];
	}
	throw std::runtime_error("Array out of bounds");
}

unsigned int RenderTexture::GetBufferCount() const
{
	return m_BufferCount;
}
