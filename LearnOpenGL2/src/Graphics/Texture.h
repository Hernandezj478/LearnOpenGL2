#pragma once

#include "Renderer.h"
#include "Common/Enums.h"

#include <string>

class Texture
{

public:
	Texture() = default;
	Texture(const std::string& filepath);
	~Texture();

	void LoadPixels();
	void Upload();

	void SyncTexture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	void SetType(TextureType type);
	void SetHDR(bool hdr);
	void SetFlipImage(bool flipImage);
	void SetInvertGreen(bool invertG);
	void SetWrapType(WrapType uvWrap);
	void SetWrapS(int wrap);
	void SetWrapT(int wrap);
	void SetWrapR(int wrap);
	void SetMinFilter(int filter);
	void SetMagFilter(int filter);
	void SetGammaCorrection(bool gamma);

	int GetWidth() const;
	int GetHeight() const;
	int GetNRChannels() const;
	TextureType GetType() const;
	unsigned int GetRendererID(unsigned int index = 0) const;
	bool IsUploaded() const { return m_Uploaded; }
private:
	bool m_GammaCorrection = false;
	bool m_FlipImage = false;
	bool m_InvertGChannel = false;
	bool m_Uploaded = false;
	bool m_HDR = false;
	unsigned int m_RendererID = 0;
	unsigned char* m_LocalBuffer = nullptr;
	float* m_HDRBuffer = nullptr;
	int m_Width, m_Height, m_nrChannels;
	std::string m_FilePath;
	TextureType m_Type = UNKNOWN;

	int m_WrapS = GL_REPEAT;
	int m_WrapT = GL_REPEAT;
	int m_WrapR = GL_REPEAT;
	int m_MinFilter = GL_LINEAR_MIPMAP_LINEAR;
	int m_MagFilter = GL_LINEAR;

	WrapType m_UVWrap;

	FileType GetFileType(const std::string& filepath);
	void InvertGChannel();
};
