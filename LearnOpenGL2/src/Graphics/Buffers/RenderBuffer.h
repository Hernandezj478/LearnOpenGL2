#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class RenderBuffer
{
private:
	unsigned int m_RendererID;

public:
	RenderBuffer();
	~RenderBuffer();
	void Bind() const;
	void Unbind() const;

	void CreateStorage(GLint internalFormat, int screenWidth, int screenHeight);
	void CreateStorageMultiSample(GLenum internalFormat, int screenWidth, int screenHeight, int samples);

	inline unsigned int GetRenderID() const { return m_RendererID; }
};

