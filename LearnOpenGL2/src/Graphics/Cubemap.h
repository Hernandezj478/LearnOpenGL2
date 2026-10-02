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

	void SetFilePathVector(std::vector<std::string> filepaths);
	void CreateCubemap();
	void CreateDepthCubeMap(const int& width, const int& height);
	void CreateHDRCubemap(const int& width, const int& height, GLuint magFilter = GL_LINEAR, GLuint minFilter = GL_LINEAR);
	void GenerateMipmap();
	inline unsigned int GetRendererID() const { return m_RendererID; }
private:
	unsigned int m_RendererID;
	unsigned char* m_LocalBuffer;

	std::vector<int> m_Width, m_Height, m_nrChannels;
	std::vector<std::string> m_FilePaths;
};

