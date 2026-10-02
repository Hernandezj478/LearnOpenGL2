#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int sizeInBytes);
	~VertexBuffer();

	void Init(const void* data, unsigned int sizeInBytes);

	void Bind() const;
	void Unbind() const;

};