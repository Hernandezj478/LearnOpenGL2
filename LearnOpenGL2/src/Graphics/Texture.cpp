#include "Texture.h"

#include "stb_image.h"

#include <sstream>
#include <vector>

Texture::Texture(const std::string& filepath) :
	m_FilePath(filepath), m_LocalBuffer(nullptr), m_HDR(false),
	m_Width(0), m_Height(0), m_nrChannels(0), m_FlipImage(true), m_InvertGChannel(false),
	m_UVWrap(REPEAT), m_GammaCorrection(false), m_Uploaded(false)
{

}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

void Texture::LoadPixels()
{
	if (m_HDR)
	{
		m_HDRBuffer = stbi_loadf(m_FilePath.c_str(), &m_Width, &m_Height, &m_nrChannels, 0);
		if (!m_HDRBuffer)
		{
			std::stringstream stream;

			stream << "[ERROR][" << (__LINE__ - 6) << "][" << __FILE__
				<< "] Could not open file " << m_FilePath << std::endl;
			throw std::runtime_error(stream.str());
			return;
		}
		
		return;
	}

	m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_nrChannels, 0);
	if (!m_LocalBuffer)
	{
		std::stringstream stream;

		stream << "[ERROR][" << (__LINE__) << "][" << __FILE__
			<< "] Could not open file " << m_FilePath << std::endl;
		throw std::runtime_error(stream.str());
	}
	if (m_FlipImage)
	{
		FlipImageVertically();
	}
	if (m_InvertGChannel)
	{
		InvertGChannel();
	}
	
}

void Texture::Upload()
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	if (m_HDR)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, m_HDRBuffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Unbind();
		m_Uploaded = true;
		if (m_HDRBuffer)
		{
			stbi_image_free(m_HDRBuffer);
		}
		return;
	}
	GLenum internalFormat, dataFormat;
	switch (m_nrChannels)
	{
	case 1:
		internalFormat = dataFormat = GL_RED;
		break;
	case 3:
		internalFormat = m_GammaCorrection ? GL_SRGB : GL_RGB;
		dataFormat = GL_RGB;
		break;
	case 4:
		internalFormat = m_GammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
		dataFormat = GL_RGBA;
		break;
	default:
		internalFormat = dataFormat = GL_RGB;
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  m_UVWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_UVWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	Unbind();
	m_Uploaded = true;
	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
	}
}

void Texture::SyncTexture()
{
	LoadPixels();
	Upload();
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetType(TextureType type)
{
	m_Type = type;
}

void Texture::SetHDR(bool hdr)
{
	m_HDR = hdr;
}

void Texture::SetFlipImage(bool flipImage)
{
	m_FlipImage = flipImage;
}

void Texture::SetInvertGreen(bool invertG)
{
	m_InvertGChannel = invertG;
}

void Texture::SetWrapType(WrapType uvWrap)
{
	m_UVWrap = uvWrap;
}

void Texture::SetGammaCorrection(bool gamma)
{
	m_GammaCorrection = gamma;
}

void Texture::FlipImageVertically()
{
	if (!m_LocalBuffer) 
	{
		return;
	}
	const int rowSize = m_Width * m_nrChannels;
	std::vector<unsigned char> tempRow(rowSize);
	int h = m_Height;
	for (int y = 0; y < (h>>1); y++)
	{
		unsigned char* rowTop = m_LocalBuffer + y * rowSize;
		unsigned char* rowBottom = m_LocalBuffer + (m_Height - 1 - y) * rowSize;

		std::memcpy(tempRow.data(), rowTop, rowSize);
		std::memcpy(rowTop, rowBottom, rowSize);
		std::memcpy(rowBottom, tempRow.data(), rowSize);
	}
}

void Texture::InvertGChannel()
{
	const int res = m_Width * m_Height;
	for (int i = 0; i < res; ++i)
	{
		const int n = i * m_nrChannels + 1;
		m_LocalBuffer[n] = 255 - m_LocalBuffer[n];
	}
}

int Texture::GetWidth() const
{
	return m_Width;
}

int Texture::GetHeight() const
{
	return m_Height;
}

int Texture::GetNRChannels() const
{
	return m_nrChannels;
}

TextureType Texture::GetType() const
{
	return m_Type;
}

unsigned int Texture::GetRendererID(unsigned int index) const
{
	return m_RendererID;
}

FileType Texture::GetFileType(const std::string& filepath)
{
	if (filepath.find(".png") != filepath.npos)
	{
		return PNG;
	}
	else if (filepath.find(".jpg") != filepath.npos) 
	{
		return JPG;
	}
	return UNSUPPORTED;
}