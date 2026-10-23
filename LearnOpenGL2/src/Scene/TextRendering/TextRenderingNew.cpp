#include "TextRenderingNew.h"

#include "Core/Window.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Renderer.h"
#include "Graphics/TextRenderer.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

TextRenderingNew::TextRenderingNew(const SceneContext& context) : Scene(context)
{
	m_TextRenderer = std::make_unique<TextRenderer>(context);
	m_TextRenderer->SetTextPosition(m_Context.Window.FramebufferSize() / glm::ivec2(2));
	m_TextRenderer->ChangeFontScale(2.0f);
	m_TextRenderer->ChangeFont(CALIBRI);

	m_FontMap[ARIAL] = "Arial";
	m_FontMap[CALIBRI] = "Calibri";
	m_FontMap[CONSOLAS] = "Consolas";
}

void TextRenderingNew::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR);
	m_TextRenderer->Draw(m_Context.Renderer, m_ScreenText);
}

void TextRenderingNew::OnGui()
{
	ImGui::InputText("Enter Text", &m_ScreenText);
	if (m_ScreenText.empty())
	{
		m_ScreenText = "OpenGL";
	}
	if (ImGui::BeginCombo("Font", m_FontMap.at(m_TextRenderer->GetCurrentFont()).c_str()))
	{
		for (auto font : m_FontMap)
		{
			bool selected = (font.first == m_TextRenderer->GetCurrentFont());
			if (ImGui::Selectable(m_FontMap[font.first].c_str(), selected))
			{
				m_TextRenderer->ChangeFont(font.first);
			}
		}
		ImGui::EndCombo();
	}
}

