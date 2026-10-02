#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Shader::Shader(const std::string& shaderPath)
{
	ShaderProgramSource source = ParseShader(shaderPath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource, source.GeometrySource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformVec2(const std::string& name, glm::vec2 value)
{
	glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniformVec3(const std::string& name, glm::vec3 value)
{
	glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniform3fv(const std::string& name, const glm::vec3* value, unsigned int count)
{
	glUniform3fv(GetUniformLocation(name), count, &value[0][0]);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	if (!stream)
	{
		std::cout << "Could not open filepath: " << filepath << std::endl;
	}
	m_FilePath = filepath;
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
	};
	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream strStream[3];
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				// set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				// set mode to fragment
				type = ShaderType::FRAGMENT;
			}
			else if (line.find("geometry") != std::string::npos)
			{
				// set mode to geometry
				type = ShaderType::GEOMETRY;
			}
		}
		else
		{
			if (type != ShaderType::NONE)
			{
				strStream[int(type)] << line << '\n';
			}
		}
	}
	//std::cout << "Vertex Shader:\n" << strStream[0].str() << "\n\n" 
	//			<< "Fragment Shader:\n" << strStream[1].str() << "\n\n" 
	//			<< "Geometry Shader:\n" << strStream[2].str() << std::endl << std::endl;
	return { strStream[(int)ShaderType::VERTEX].str(), 
			 strStream[(int)ShaderType::FRAGMENT].str(), 
			 strStream[(int)ShaderType::GEOMETRY].str()};
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	GLint success;
	GLchar infoLog[1024];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(id, 1024, NULL, infoLog);
		std::cout << m_FilePath << std::endl;
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " <<
			(type == (unsigned int)GL_VERTEX_SHADER ? "VERTEX" : 
				type == (unsigned int)GL_FRAGMENT_SHADER ? "FRAGMENT" : "GEOMETRY") <<
			"\n" << infoLog << "\n" << "---------------------------------------------" << std::endl;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader,
									const std::string& fragmentShader, 
									const std::string& geometryShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);


	unsigned int gs;
	if (!geometryShader.empty())
	{
		gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
	}

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	if (!geometryShader.empty()) glAttachShader(program, gs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

GLint Shader::GetUniformLocation(const std::string& name) const
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	m_UniformLocationCache[name] = location;
	return location;
}