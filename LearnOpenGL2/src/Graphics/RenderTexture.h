#pragma once

#include "Common/Enums.h"
#include <string>

class RenderTexture
{
public:
	explicit RenderTexture(unsigned int width,unsigned int height, unsigned int bufferCount = 1);
	~RenderTexture();

	/*
	*! @brief Create a buffer
	* @param interlaFormat Specifies the number of color components in texture
	* @param format Specifies the format of the pixel data
	* @param type Specified the data type of the pixel
	*/
	void CreateTextureBuffer(GLint internalFormat, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_INT, const void* data = NULL);
	void AddTextureData(GLenum format, GLenum type, const void* data);

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