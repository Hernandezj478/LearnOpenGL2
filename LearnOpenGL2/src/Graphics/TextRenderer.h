#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Common/Fonts_Enum.h"
#include "Graphics/Shapes/Glyph.h"
#include "Graphics/Font.h"

class Font;
class Glyph;
class Renderer;
class Shader;

struct SceneContext;

class TextRenderer
{
public:
	TextRenderer(const SceneContext& context);
	~TextRenderer();

	void Draw(const Renderer& renderer, const std::string& text);

	void SetTextPosition(glm::vec2 position);
	void SetTextColor(glm::vec3 color);

	void ChangeFont(FontFamily fontFamily);
	void ChangeFontScale(float scale);

	const FontFamily GetCurrentFont() const;
private:
	glm::vec2 m_TextPosition;
	glm::vec3 m_TextColor;
	std::unique_ptr<Font> m_Font;
	std::unique_ptr<Glyph> m_Glyph;
	std::unique_ptr<Shader> m_Shader;

	std::vector<GlyphInstance> m_GlyphInstances;

	float m_FontScale = 1.0f;
};