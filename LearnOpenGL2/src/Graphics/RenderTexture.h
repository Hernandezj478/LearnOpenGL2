#pragma once

#include "Common/Enums.h"
#include <string>

class RenderTexture
{
public:
	explicit RenderTexture(unsigned int bufferCount = 1);
	~RenderTexture();

	void CreateColorBufferMSAA(int screenWidth, int screenHeight, int samples, GLenum internalFormat = GL_RGB);
	void CreateColorbufferHDR(int screenWidth, int screenHeight, GLint internalFormat = GL_RGBA16F, WrapType uvWrap = ECLAMP, GLenum filter = GL_LINEAR);
	void CreateLUTBuffer(int width, int height, GLint internalFormat, GLenum format, GLenum type);
	void CreateColorbuffer(int screenWidth, int screenHeight, GLenum internalFormat = GL_RGB);
	void CreateDepthbuffer(int screenWidth, int screenHeight);
	void CreateDepthbufferTexture(int screenWidth, int screenHeight, WrapType uvWrap = REPEAT, GLenum filter = GL_NEAREST);
	void CreateStencilbuffer(int screenWidth, int screenHeight);
	void CreteDepthStencilbuffer(int screenWidth, int screenHeight);
	void CreateBorder(glm::vec4 color);

	void Bind(unsigned int slot = 0, unsigned int index = 0) const;
	void Unbind() const;
	void SetTarget(TextureTarget target);
	void SetMultiAttachment(bool multiattach);
	unsigned int GetRendererID(unsigned int index = 0) const;
	unsigned int GetBufferCount() const;
	GLenum GetTarget() const { return (m_Target == TextureTarget::Texture2D) ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE; }
	bool GetMultiAttachment() const { return m_MultiAttachment; }

	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

private:
	unsigned int m_BufferCount;
	unsigned int* m_RendererID;
	int m_Width = 0;
	int m_Height = 0;
	bool m_MultiAttachment;
	TextureTarget m_Target;
};