#include "Cubemap.h"
#include "Graphics/Texture.h"

#include "stb_image.h"
#include <sstream>
#include <iostream>

Cubemap::Cubemap() : m_LocalBuffer(nullptr)
{
	glGenTextures(1, &m_RendererID);
}

Cubemap::Cubemap(std::vector<std::string> filepaths)
{
	SetFilePathVector(filepaths);
	CreateCubemap();
}

Cubemap::~Cubemap()
{
}

void Cubemap::SetFilePathVector(std::vector<std::string> filepaths)
{
	m_FilePaths = filepaths;
}

void Cubemap::CreateCubemap()
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	if (m_FilePaths.empty())
	{
		std::cerr << "[ERROR][" << __FILE__ << "][" << __FUNCTION__ << "]" <<
			"No files found." << std::endl;
		return;
	}
	stbi_set_flip_vertically_on_load(false);
	for (unsigned int i = 0; i < m_FilePaths.size(); i++)
	{
		int width, height, nrChannels;
		m_LocalBuffer = stbi_load(m_FilePaths[i].c_str(), &width, &height, &nrChannels, 0);

		if (!m_LocalBuffer)
		{
			std::stringstream stream;

			stream << "[ERROR][" << (__LINE__ - 6) << "][" << __FILE__
				<< "] Could not open file " << m_FilePaths[i]
				<< " - Please verify file." << std::endl;
			std::cerr << stream.str();
		}

		m_Width.push_back(width);
		m_Height.push_back(height);
		m_nrChannels.push_back(nrChannels);

		GLenum format;
		switch (m_nrChannels[i])
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			format = GL_RGB;
			break;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, 
			m_Width[i], m_Height[i], 0, format, GL_UNSIGNED_BYTE, m_LocalBuffer);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::CreateDepthCubeMap(const int& width, const int& height)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::CreateHDRCubemap(const int& width, const int& height, GLuint magFilter, GLuint minFilter)
{
	m_Width.push_back(width);
	m_Height.push_back(height);
	m_nrChannels.push_back(4);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
}

void Cubemap::GenerateMipmap()
{
	Bind();
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void Cubemap::Bind(const unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
}
void Cubemap::Unbind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

