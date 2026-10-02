#pragma once

#include "BufferObject.h"
#include <string>
#include <vector>
class Shader;

class UniformBuffer : public BufferObject
{
private:
	unsigned int m_BindPoint;
	std::string m_BlockName;
	std::vector<Shader*> m_Shaders;

public:
	UniformBuffer(const std::string& blockName, unsigned int bindpoint);

	void AddShader(Shader& shader);

	void SetData(unsigned int offset, unsigned int size, void* data);
	
	void BindBlock(Shader& shader, const char* name);
	unsigned int GetUniformIndex(Shader& shader, const char* name);


	void BindBase();
	void BindRange(unsigned int offset, unsigned int size);

	unsigned int GetBindPoint() const { return m_BindPoint; }
	const std::string& GetBlockName() const { return m_BlockName; }
};