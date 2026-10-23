#pragma once

#include "Scene/Scene.h"
#include "Common/Fonts_Enum.h"

#include <memory>
#include <string>
#include <map>

class TextRenderer;

class TextRenderingNew : public Scene
{
public:
	TextRenderingNew(const SceneContext& context);

	void Render() override;
	void OnGui() override;
private:

	std::unique_ptr<TextRenderer> m_TextRenderer;
	std::string m_ScreenText = "OpenGL";
	std::map<FontFamily, std::string> m_FontMap;
};