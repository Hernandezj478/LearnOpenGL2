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
	void SetGammaCorrection(bool gamma);

	void FlipImageVertically();
	void InvertGChannel();
	int GetWidth() const;
	int GetHeight() const;
	int GetNRChannels() const;
	TextureType GetType() const;
	unsigned int GetRendererID(unsigned int index = 0) const;
	bool IsUploaded() const { return m_Uploaded; }
private:
	bool m_GammaCorrection;
	bool m_FlipImage;
	bool m_InvertGChannel;
	bool m_Uploaded;
	bool m_HDR;
	unsigned int m_RendererID;
	unsigned char* m_LocalBuffer = nullptr;
	float* m_HDRBuffer = nullptr;
	int m_Width, m_Height, m_nrChannels;
	std::string m_FilePath;
	TextureType m_Type = UNKNOWN;
	WrapType m_UVWrap;

	FileType GetFileType(const std::string& filepath);
};
