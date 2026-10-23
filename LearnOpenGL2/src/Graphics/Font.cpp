#include "Font.h"

#include "Graphics/RenderTexture.h"
#include <stdexcept>

#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

Font::Font(FontFamily font)
{
	m_Fonts[ARIAL] = "fonts/Arial/arial.ttf";
	m_Fonts[CALIBRI] = "fonts/Calibri/calibri.ttf";
	m_Fonts[CONSOLAS] = "fonts/Consolas/consolab.ttf";
	m_CurrentFont = font;

	m_Atlas = std::vector<unsigned char>(m_AtlasWidth * m_AtlasHeight, 0);
	m_AtlasTexture = std::make_unique<RenderTexture>(m_AtlasWidth, m_AtlasHeight);
	LoadFont();
}

Font::~Font() = default;

void Font::Bind(unsigned int slot) const
{
	m_AtlasTexture->Bind(slot);
}

void Font::ChangeFont(FontFamily font)
{
	m_CurrentFont = font;
	LoadFont();
}

void Font::LoadFont()
{
	m_Characters.clear();

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
	}
	FT_Face face;
	if (FT_New_Face(ft, m_Fonts.at(m_CurrentFont).c_str(), 0, &face))
	{
		throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int penX = 0;
	unsigned int penY = 0;
	unsigned int rowHeight = 0;

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			throw std::runtime_error("Could not load glyph: " + c);
		}
		const FT_Bitmap& bitmap = face->glyph->bitmap;

		if (penX + bitmap.width > m_AtlasWidth)
		{
			penX = 0;
			penY += rowHeight;
			rowHeight = 0;
		}
		if (penY + bitmap.rows > m_AtlasHeight)
		{
			throw std::runtime_error("Atlas too small");
		}
		for (unsigned int row = 0; row < bitmap.rows; row++)
		{
			unsigned int dstY = penY + (bitmap.rows - 1 - row);
			for (unsigned int col = 0; col < bitmap.width; col++)
			{
				m_Atlas[dstY * m_AtlasWidth + penX + col] = bitmap.buffer[row * bitmap.pitch + col];
			}
		}
		Character character;
		character.Advance = face->glyph->advance.x;
		character.Size = glm::ivec2(bitmap.width, bitmap.rows);
		character.Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		character.UVMin = glm::vec2(penX / static_cast<float>(m_AtlasWidth), penY / static_cast<float>(m_AtlasHeight));
		character.UVMax = glm::vec2((penX + bitmap.width) / static_cast<float>(m_AtlasWidth), (penY + bitmap.rows) / static_cast<float>(m_AtlasHeight));
		m_Characters.insert(std::pair<unsigned char, Character>(c, character));

		penX += bitmap.width;
		rowHeight = std::max(rowHeight, bitmap.rows);
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	m_AtlasTexture->SetWrapS(GL_CLAMP_TO_EDGE);
	m_AtlasTexture->SetWrapT(GL_CLAMP_TO_EDGE);
	m_AtlasTexture->SetMinFilter(GL_NEAREST);
	m_AtlasTexture->SetMagFilter(GL_NEAREST);
	m_AtlasTexture->CreateTextureBuffer(GL_RED, GL_RED, GL_UNSIGNED_BYTE, m_Atlas.data());
}
