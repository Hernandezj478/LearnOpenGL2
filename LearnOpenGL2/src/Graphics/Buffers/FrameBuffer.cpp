#include "FrameBuffer.h"

#include "Graphics/RenderTexture.h"
#include "Graphics/Cubemap.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>

FrameBuffer::FrameBuffer(unsigned int count) : m_BufferCount(count)
{
	m_RendererID = new unsigned int[m_BufferCount];
	glGenFramebuffers(m_BufferCount, m_RendererID);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(m_BufferCount, m_RendererID);
	delete[] m_RendererID;
}

void FrameBuffer::Bind(unsigned int index) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID[index]);
}
void FrameBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BindRead(unsigned int index) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID[index]);
}

void FrameBuffer::BindDraw(unsigned int index) const
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID[index]);
}

void FrameBuffer::NoColorData() const
{
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

void FrameBuffer::AttachColorBuffer(const RenderTexture& color)
{
	int fbCount = 0;
	for (unsigned int i = 0; i < color.GetBufferCount(); ++i)
	{
		if (m_BufferCount > 1)
		{
			fbCount = i;
		}
		Bind(fbCount);
		unsigned int attachmentPoint = GL_COLOR_ATTACHMENT0 + (color.GetMultiAttachment() ? i : 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, color.GetTarget(), color.GetRendererID(i), 0);
	}
}
void FrameBuffer::AttachColorBuffer(const RenderTexture& color, unsigned int slot)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, color.GetTarget(), color.GetRendererID(), 0);
}
/*
	NOTE:
	For the attachment functions, we might need to change to adapt to new format using pointer rendererid.
	The way its currently set up assumes we are only creating and binding 1 framebuffer and have only 1 texture buffer
*/
void FrameBuffer::AttachDepthBuffer(const RenderTexture& depth)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.GetRendererID(), 0);
}

void FrameBuffer::AttachStencilBuffer(const RenderTexture& stencil)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil.GetRendererID(), 0);
}

void FrameBuffer::AttachDepthStencilBuffer(const RenderTexture& depthStencil)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil.GetRendererID(), 0);
}

void FrameBuffer::AttachRenderBuffer(const RenderBuffer& renderBuffer, GLint attachment)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderBuffer.GetRenderID());
}

void FrameBuffer::AttachDepthCubeMap(const Cubemap& cubeMap)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubeMap.GetRendererID(), 0);
}

void FrameBuffer::AttachCubemap(const Cubemap& cubemap, unsigned int position, unsigned int slot, unsigned int level)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_CUBE_MAP_POSITIVE_X + position, cubemap.GetRendererID(), level);
}

//------------------------------------------------------------------------------------------------------------
// This is just to test against opengl course
void FrameBuffer::CreateAndAttachMRT(RenderTexture& color, int width, int height, GLenum internalFormat)
{
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID[i]);
		glBindTexture(GL_TEXTURE_2D, color.GetRendererID(i));
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.GetRendererID(i), 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Framebuffer not complete!" << std::endl;
		}
	}
}

void FrameBuffer::CreateAndAttachMRTColorBuffer(RenderTexture& color, int width, int height)
{
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, color.GetRendererID(i));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color.GetRendererID(i), 0);
	}
}
//------------------------------------------------------------------------------------------------------------


void FrameBuffer::ConfigureAttachments(unsigned int* attachments, unsigned int count)
{
	glDrawBuffers(count, attachments);
}

bool FrameBuffer::FrameBufferComplete()
{
	for (unsigned int i = 0; i < m_BufferCount; ++i)
	{
		Bind(i);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "[ERROR][" << __FILE__ << "][" << __FUNCTION__ << "]: " <<
				"Framebuffer " << i << " incomplete" << std::endl;
			return false;
		}
	}
	return true;
}

void FrameBuffer::BlitColor(const FrameBuffer& dst, int screenWidth, int screenHeight, GLenum filter)
{
	BindRead();
	dst.BindDraw();
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, filter);
}
