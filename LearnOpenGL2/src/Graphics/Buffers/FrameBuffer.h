#pragma once

#include "RenderBuffer.h"
#include <vector>

class RenderTexture;
class Cubemap;

class FrameBuffer
{
public:
	FrameBuffer(unsigned int count = 1);
	~FrameBuffer();

	void Bind(unsigned int index = 0) const;
	void Unbind() const;

	void BindRead(unsigned int index = 0) const;
	void BindDraw(unsigned int index = 0) const;
	
	void NoColorData() const;

	void AttachColorBuffer(const RenderTexture& color);	// Used for both LDR and HDR
	void AttachColorBuffer(const RenderTexture& color, unsigned int slot);
	void AttachDepthBuffer(const RenderTexture& depth);
	void AttachStencilBuffer(const RenderTexture& stencil);
	void AttachDepthStencilBuffer(const RenderTexture& depthStencil);
	void AttachRenderBuffer(const RenderBuffer& renderBuffer, GLint attachment);
	void AttachDepthCubeMap(const Cubemap& cubeMap);
	void AttachCubemap(const Cubemap& cubemap, unsigned int position, unsigned int slot = 0, unsigned int level = 0);

	void CreateAndAttachMRT(RenderTexture& color, int width, int height, GLenum internalFormat);
	void CreateAndAttachMRTColorBuffer(RenderTexture& color, int width, int height);

	//void ConfigureColorAttachments(const RenderTexture& renderTexture);
	void ConfigureColorAttachments(int attachmentCount);

	bool FrameBufferComplete();

	void BlitBuffer(FrameBuffer* dst, int screenWidth, int screenHeight, int mask = GL_COLOR_BUFFER_BIT, GLenum filter = GL_NEAREST);

	inline unsigned int GetRendererID(unsigned int index = 0) const { return m_RendererID[index]; }
private:
	unsigned int* m_RendererID;
	unsigned int m_BufferCount;

	std::vector<unsigned int> m_ColorAttachments;
};
