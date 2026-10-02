#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderParser;

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	std::string GeometrySource;
};

class Shader
{
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;
	
	//Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformVec2(const std::string& name, glm::vec2 value);
	void SetUniformVec3(const std::string& name, glm::vec3 value);
	void SetUniform3fv(const std::string& name, const glm::vec3* value, unsigned int count = 1);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	void SetUniformMat3f(const std::string& name, const glm::mat3& matrix);

	//Getters
	unsigned int GetRendererID() const { return m_RendererID; }

private:
	std::string m_FilePath;
	unsigned int m_RendererID;

	mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;

	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, 
								const std::string& fragmentShader, 
									const std::string& geometryShader = "");

	GLint GetUniformLocation(const std::string& name) const;

};