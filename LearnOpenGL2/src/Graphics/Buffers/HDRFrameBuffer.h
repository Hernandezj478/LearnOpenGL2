#pragma once

class HDRFramebuffer
{
private:
	unsigned int m_FBO;
	unsigned int m_ColorBuffers[2];
	unsigned int m_RBO;
	int m_Width, m_Height;

public:
	HDRFramebuffer(int width, int height);
	~HDRFramebuffer();

	void Bind() const;
	void Unbind() const;

	unsigned int GetSceenColorBuffer() const { return m_ColorBuffers[0]; }
	unsigned int GetBrightColorBuffer() const { return m_ColorBuffers[1]; }

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

	bool IsComplete() const;
};