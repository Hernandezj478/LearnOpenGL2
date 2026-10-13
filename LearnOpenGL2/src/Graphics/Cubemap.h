#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>
#include <string>


class Cubemap
{
public:
	Cubemap();
	Cubemap(std::vector<std::string> filepaths);
	~Cubemap();

	void Bind(const unsigned int slot = 0) const;
	void Unbind() const;


	void CreateCubemap();
	void CreateDepthCubeMap(const int& width, const int& height);
	void CreateHDRCubemap(const int& width, const int& height);
	void GenerateMipmap();
	void SetFilePathVector(std::vector<std::string> filepaths);
	void SetMinFilter(int minFilter);
	void SetMagFilter(int magFilter);
	void SetWrapS(int wrapS);
	void SetWrapT(int wrapT);
	void SetWrapR(int wrapR);


	inline unsigned int GetRendererID() const { return m_RendererID; }
private:
	unsigned int m_RendererID;
	unsigned char* m_LocalBuffer;

	int m_MinFilter = GL_LINEAR;
	int m_MagFilter = GL_LINEAR;
	int m_WrapS = GL_CLAMP_TO_EDGE;
	int m_WrapT = GL_CLAMP_TO_EDGE;
	int m_WrapR = GL_CLAMP_TO_EDGE;

	std::vector<int> m_Width, m_Height, m_nrChannels;
	std::vector<std::string> m_FilePaths;
};

