#include "TextRenderer.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Renderer.h"
#include "Graphics/Font.h"
#include "Graphics/Shader.h"
#include "Graphics/Shapes/Glyph.h"
#include "Scene/SceneContext.h"
#include "Core/Window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TextRenderer::TextRenderer(const SceneContext& context)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_TextPosition = glm::vec2(0.0);
	m_TextColor = WHITE;
	m_Font = std::make_unique<Font>();
	m_Glyph = std::make_unique<Glyph>();
	m_Shader = std::make_unique<Shader>("res/shaders/TextRenderer/TextRenderer.glsl");
	glm::ivec2 size = context.Window.FramebufferSize();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(size.x), 0.0f, static_cast<float>(size.y));
	m_Shader->Bind();
	m_Shader->SetUniform1i("atlas", 0);
	m_Shader->SetUniformMat4f("projection", projection);
}

TextRenderer::~TextRenderer() = default;

void TextRenderer::Draw(const Renderer & renderer, const std::string& text)
{
	m_GlyphInstances.clear();
	if (text.empty())
	{
		return;
	}
	float penX = m_TextPosition.x;
	for (unsigned char c : text)
	{
		const Character& character = m_Font->GetCharacter(c);
		GlyphInstance glyph;
		glm::mat4 model(1.0);
		model = glm::translate(model, glm::vec3(
									penX + character.Bearing.x * m_FontScale, 
									m_TextPosition.y - (character.Size.y - character.Bearing.y) * m_FontScale,
									0.0f));
		model = glm::scale(model, glm::vec3(character.Size.x * m_FontScale, character.Size.y * m_FontScale, 1.0f));
		glyph.transform = model;
		glyph.UVData = glm::vec4(character.UVMin, character.UVMax);
		m_GlyphInstances.push_back(glyph);
		penX += (character.Advance >> 6) * m_FontScale;
	}
	m_Glyph->CreateInstance(m_GlyphInstances);

	m_Shader->Bind();
	m_Shader->SetUniformVec3("textColor", m_TextColor);
	m_Font->Bind();
	m_Glyph->Draw(*m_Shader, renderer);
}

void TextRenderer::SetTextPosition(glm::vec2 position)
{
	m_TextPosition = position;
}

void TextRenderer::SetTextColor(glm::vec3 color)
{
	m_TextColor = color;
}

void TextRenderer::ChangeFont(FontFamily fontFamily)
{
	m_Font->ChangeFont(fontFamily);
}

void TextRenderer::ChangeFontScale(float scale)
{
	m_FontScale = scale;
}

const FontFamily TextRenderer::GetCurrentFont() const
{
	return m_Font->GetCurrentFont();
}
