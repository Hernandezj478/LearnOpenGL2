#include "UniformBuffer.h"
#include "Graphics/Shader.h"


UniformBuffer::UniformBuffer(const std::string& blockName, unsigned int bindpoint)
	: BufferObject(GL_UNIFORM_BUFFER), m_BindPoint(bindpoint), m_BlockName(blockName)
{
}

void UniformBuffer::AddShader(Shader& shader)
{
	BindBlock(shader, m_BlockName.c_str());
	m_Shaders.push_back(&shader);
}

void UniformBuffer::SetData(unsigned int offset, unsigned int size, void* data)
{
	Bind();
	SubBufferData(offset, size, data);
	Unbind();
}

void UniformBuffer::BindBlock(Shader& shader, const char* name)
{
	unsigned int index = GetUniformIndex(shader, name);
	if (index == GL_INVALID_INDEX)
	{
		std::cout << "[WARNING] Uniform Block '" << name << "' not found in shader program" << std::endl;
	}
	glUniformBlockBinding(shader.GetRendererID(), index, m_BindPoint);
}

unsigned int  UniformBuffer::GetUniformIndex(Shader& shader, const char* name)
{
	return glGetUniformBlockIndex(shader.GetRendererID(), name);
}

void UniformBuffer::BindBase()
{
	glBindBufferBase(GetTargetBuffer(), m_BindPoint, GetRendererID());
}

void UniformBuffer::BindRange(unsigned int offset, unsigned int size)
{
	glBindBufferRange(GetTargetBuffer(), m_BindPoint, GetRendererID(), offset, size);
}

