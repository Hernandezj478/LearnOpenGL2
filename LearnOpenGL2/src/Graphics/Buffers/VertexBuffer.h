#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int sizeInBytes, int usage);
	~VertexBuffer();

	void Init(const void* data, unsigned int sizeInBytes);

	void Bind() const;
	void Unbind() const;

	void AddSubData(unsigned int offset, unsigned int size, void* data);
};