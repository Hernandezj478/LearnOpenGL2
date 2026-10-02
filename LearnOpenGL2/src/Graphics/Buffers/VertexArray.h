#pragma once

class VertexBuffer;
class VertexBufferLayout;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout);
	void AddInstancedBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout& layout, unsigned int stride);
	void Bind() const;
	void Unbind() const;

	unsigned int GetRendererID() const { return m_RendererID; }
private:
	unsigned int m_RendererID;
	unsigned int m_AttribIndexOffset = 0;
};