#pragma once

#include <memory>
#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Common/Fonts_Enum.h"

class RenderTexture;

struct Character
{
	signed long Advance = 0;
	glm::ivec2 Size = glm::ivec2(0);
	glm::ivec2 Bearing = glm::ivec2(0);

	glm::vec2 UVMin = glm::vec2(0.0f);
	glm::vec2 UVMax = glm::vec2(0.0f);
};

class Font
{
public:
	Font(FontFamily font = ARIAL);
	~Font();

	void Bind(unsigned int slot = 0) const;

	void ChangeFont(FontFamily font);
	std::vector<unsigned char> GetAtlas() const { return m_Atlas; }
	const Character& GetCharacter(unsigned char c) { return m_Characters.at(c); }
	const FontFamily GetCurrentFont() const { return m_CurrentFont; }
private:
	std::map<unsigned char, Character> m_Characters;
	std::map<FontFamily, std::string> m_Fonts;
	std::vector<unsigned char> m_Atlas;

	std::unique_ptr<RenderTexture> m_AtlasTexture;

	unsigned int m_AtlasWidth = 512;
	unsigned int m_AtlasHeight = 512;

	FontFamily m_CurrentFont;
	FontType m_CurrentFontType = REGULAR;

	void LoadFont();
};