#include "TextRendering.h"

#include "Core/Window.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"

#include "Graphics/Buffers/VertexArray.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Buffers/VertexBufferLayout.h"

#include "Scene/SceneContext.h"


#include <glad/glad.h>

TextRendering::TextRendering(const SceneContext& context) : Scene(context)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");
	}
	FT_Face face;
	if (FT_New_Face(ft, "fonts/Arial/arial.ttf", 0, &face))
	{
		FT_Done_Face(face);
		throw std::runtime_error("ERROR::FREETYPE: Failed to load font");
	}
	FT_Set_Pixel_Sizes(face, 0, 12);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			throw std::runtime_error("ERROR::FREETYPE: Failed to load glyph");
		}
		std::shared_ptr<RenderTexture> texture = std::make_shared<RenderTexture>(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		texture->SetMinFilter(GL_LINEAR);
		texture->SetMagFilter(GL_LINEAR);
		texture->SetWrapS(GL_CLAMP_TO_EDGE);
		texture->SetWrapT(GL_CLAMP_TO_EDGE);
		texture->CreateTextureBuffer(GL_RED, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		Character character =
		{
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	m_Shader = std::make_unique<Shader>("res/shaders/TextRendering/TextRendering.glsl");
	m_VAO = std::make_unique<VertexArray>();
	m_VBO = std::make_unique<VertexBuffer>((void*)0, sizeof(float) * 6 * 4, GL_DYNAMIC_DRAW);
	m_Layout = std::make_unique<VertexBufferLayout>();
	m_Layout->Push<float>(4);

	m_VAO->Bind();
	m_VAO->AddBuffer(*m_VBO, *m_Layout);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::ivec2 size = m_Context.Window.FramebufferSize();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(size.x), 0.0f, static_cast<float>(size.y));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniform1i("text", 0);
}

void TextRendering::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR);
	RenderText("OpenGL", m_Context.Window.FramebufferSize().x / 2.0, m_Context.Window.FramebufferSize().y / 2.0, 1.0f, RED);
}

void TextRendering::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	m_Shader->Bind();
	m_Shader->SetUniformVec3("textColor", color);

	glActiveTexture(GL_TEXTURE0);
	m_VAO->Bind();

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		float xPos = x + ch.Bearing.x * scale;
		float yPos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float vertices[6][4] =
		{
			{xPos,     yPos + h, 0.0f, 0.0f},
			{xPos,     yPos,     0.0f, 1.0f},
			{xPos + w, yPos,     1.0f, 1.0f},
			
			{xPos,     yPos + h, 0.0f, 0.0f},
			{xPos + w, yPos,     1.0f, 1.0f},
			{xPos + w, yPos + h, 1.0f, 0.0f}
		};
		ch.Texture->Bind();
		m_VBO->Bind();
		m_VBO->AddSubData(0, sizeof(vertices), vertices);
		m_VBO->Unbind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale;
	}
	m_VAO->Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
}
