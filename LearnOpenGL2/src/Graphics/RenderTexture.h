#pragma once

#include "Common/Enums.h"
#include <string>

class RenderTexture
{
public:
	explicit RenderTexture(int width, int height, unsigned int bufferCount = 1);
	~RenderTexture();

	void CreateTextureBuffer(GLint internalFormat, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_INT);
	
	// TODO: Remove old functions, no longer needed
	/*void CreateColorBufferMSAA(int samples, GLenum internalFormat = GL_RGB);
	void CreateColorbufferHDR(GLint internalFormat = GL_RGBA16F, WrapType uvWrap = ECLAMP, GLenum filter = GL_LINEAR);
	void CreateLUTBuffer(GLint internalFormat, GLenum format, GLenum type);
	void CreateColorbuffer(GLenum internalFormat = GL_RGB);
	void CreateDepthbuffer(int screenWidth, int screenHeight);
	void CreateDepthbufferTexture(int screenWidth, int screenHeight, WrapType uvWrap = REPEAT, GLenum filter = GL_NEAREST);
	void CreateStencilbuffer(int screenWidth, int screenHeight);
	void CreteDepthStencilbuffer(int screenWidth, int screenHeight);*/
	void CreateBorder(glm::vec4 color);

	void Bind(unsigned int slot = 0, unsigned int index = 0) const;
	void Unbind() const;
	
	void SetTarget(int target);
	void SetMultiAttachment(bool multiattach);
	void SetMinFilter(int filter);
	void SetMagFilter(int filter);
	void SetWrapS(int wrap);
	void SetWrapT(int wrap);
	void SetWrapR(int wrap);
	void SetSamples(int samples);
	
	unsigned int GetRendererID(unsigned int index = 0) const;
	unsigned int GetBufferCount() const;
	int GetTarget() const { return m_Target; }
	bool GetMultiAttachment() const { return m_MultiAttachment; }

	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }




private:
	unsigned int m_BufferCount;
	unsigned int* m_RendererID;
	int m_Width = 0;
	int m_Height = 0;
	bool m_MultiAttachment = false;

	int m_Target = GL_TEXTURE_2D;
	int m_MinFilter = GL_LINEAR;
	int m_MagFilter = GL_LINEAR;
	int m_WrapS = GL_CLAMP_TO_EDGE;
	int m_WrapT = GL_CLAMP_TO_EDGE;
	int m_WrapR = GL_CLAMP_TO_EDGE;
	int m_MSAASamples = 4;
};