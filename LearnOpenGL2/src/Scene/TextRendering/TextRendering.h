#pragma once
#include "Scene/Scene3D.h"

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H

class RenderTexture;
class Shader;
class VertexArray;
class VertexBuffer;
class VertexBufferLayout;

class TextRendering : public Scene
{
public:
	TextRendering(const SceneContext& context);

	void Render() override;
private:
	struct Character
	{
		std::shared_ptr<RenderTexture> Texture;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		unsigned int Advance;
	};
	std::map<char, Character> Characters;

	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<VertexBufferLayout> m_Layout;

	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
};