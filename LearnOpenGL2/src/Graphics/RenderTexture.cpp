#include "RenderTexture.h"
#include <stdexcept>

RenderTexture::RenderTexture(int width, int height, unsigned int bufferCount) : 
	m_BufferCount(bufferCount), m_Width(width), m_Height(height)
{
	m_RendererID = new unsigned int[bufferCount];
	glGenTextures(m_BufferCount, m_RendererID);
}

RenderTexture::~RenderTexture()
{
	delete m_RendererID;
}

void RenderTexture::CreateTextureBuffer(GLint internalFormat, GLenum format, GLenum type)
{
	if (m_Target == GL_TEXTURE_2D)
	{
		for (unsigned int i = 0; i < m_BufferCount; ++i)
		{
			glBindTexture(m_Target, m_RendererID[i]);
			glTexImage2D(m_Target, 0, internalFormat, m_Width, m_Height, 0, format, type, NULL);

			glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, m_MinFilter);
			glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, m_MagFilter);
			glTexParameterf(m_Target, GL_TEXTURE_WRAP_S, m_WrapS);
			glTexParameterf(m_Target, GL_TEXTURE_WRAP_T, m_WrapT);
			glTexParameterf(m_Target, GL_TEXTURE_WRAP_R, m_WrapR);
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_BufferCount; ++i)
		{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID[i]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_MSAASamples, internalFormat, m_Width, m_Height, GL_TRUE);
		}
	}
}

//void RenderTexture::CreateColorBufferMSAA(int samples, GLenum internalFormat)
//{
//	m_Target = GL_TEXTURE_2D_MULTISAMPLE;
//	for (unsigned int i = 0; i < m_BufferCount; ++i)
//	{
//		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID[i]);
//		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, m_Width, m_Height, GL_TRUE);
//	}
//	Unbind();
//}
//
//void RenderTexture::CreateColorbufferHDR(GLint internalFormat, WrapType uvWrap, GLenum filter)
//{ 
//	m_Target = GL_TEXTURE_2D;
//	for (unsigned int i = 0; i < m_BufferCount; ++i)
//	{
//		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uvWrap);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, uvWrap);
//	}
//}
//
//void RenderTexture::CreateLUTBuffer(GLint internalFormat, GLenum format, GLenum type)
//{
//	m_Target = GL_TEXTURE_2D;
//
//	glBindTexture(GL_TEXTURE_2D, m_RendererID[0]);
//	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, 0);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//}
//
//void RenderTexture::CreateColorbuffer(GLenum internalFormat)
//{
//	m_Target = GL_TEXTURE_2D;
//	for (unsigned int i = 0; i < m_BufferCount; ++i)
//	{
//		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, internalFormat, GL_UNSIGNED_BYTE, NULL);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	}
//}
//
//void RenderTexture::CreateDepthbuffer(int screenWidth, int screenHeight)
//{
//	m_Width = screenWidth;
//	m_Height = screenHeight;
//	for (unsigned int i = 0; i < m_BufferCount; ++i)
//	{
//		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	}
//}
//
//void RenderTexture::CreateDepthbufferTexture(int screenWidth, int screenHeight, WrapType uvWrap, GLenum filter)
//{
//	m_Width = screenWidth;
//	m_Height = screenHeight;
//	for (unsigned int i = 0; i < m_BufferCount; i++)
//	{
//		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uvWrap);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, uvWrap);
//	}
//}
//
//void RenderTexture::CreateStencilbuffer(int screenWidth, int screenHeight)
//{
//	m_Width = screenWidth;
//	m_Height = screenHeight;
//	for (unsigned int i = 0; i < m_BufferCount; ++i)
//	{
//		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);
//
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, m_Width, m_Height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_INT, NULL);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	}
//}
//
//void RenderTexture::CreteDepthStencilbuffer(int screenWidth, int screenHeight)
//{
//	m_Width = screenWidth;
//	m_Height = screenHeight;
//
//	for (unsigned int i = 0; i < m_BufferCount; ++i)
//	{
//		glBindTexture(GL_TEXTURE_2D, m_RendererID[i]);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
//			m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	}
//}

void RenderTexture::CreateBorder(glm::vec4 color)
{
	Bind();
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &color[0]);
}

void RenderTexture::Bind(unsigned int slot, unsigned int index) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	// Lets make sure the index is in bounds of the array
	if (index >= m_BufferCount)
	{
		throw std::runtime_error("Array out of bounds");
	}
	glBindTexture(GetTarget(), m_RendererID[index]);
}

void RenderTexture::Unbind() const
{
	glBindTexture(GetTarget(), 0);
}

void RenderTexture::SetTarget(int target)
{
	m_Target = target;
}

void RenderTexture::SetMultiAttachment(bool multiattach)
{
	m_MultiAttachment = multiattach;
}

void RenderTexture::SetMinFilter(int filter)
{
	m_MinFilter = filter;
}

void RenderTexture::SetMagFilter(int filter)
{
	m_MagFilter = filter;
}

void RenderTexture::SetWrapS(int wrap)
{
	m_WrapS = wrap;
}

void RenderTexture::SetWrapT(int wrap)
{
	m_WrapT = wrap;
}

void RenderTexture::SetWrapR(int wrap)
{
	m_WrapR = wrap;
}

void RenderTexture::SetSamples(int samples)
{
	m_MSAASamples = samples;
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
