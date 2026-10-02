#pragma once

#include <glad/glad.h>

class BufferObject
{
private:
	unsigned int m_RendererID;
	unsigned int m_TargetBuffer;

protected:
	unsigned int GetRendererID() const { return m_RendererID; }
	unsigned int GetTargetBuffer() const { return m_TargetBuffer; }
public:
	BufferObject(unsigned int target);
	~BufferObject();


	void Bind() const;
	void Unbind() const;

	void CreateBufferObject(void* data, unsigned int byteSize);
	void SubBufferData(unsigned int offset, unsigned int byteSize, void* data);

};

