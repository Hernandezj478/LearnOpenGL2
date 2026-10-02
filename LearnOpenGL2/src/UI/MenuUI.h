#pragma once

#include <glm/glm.hpp>

#include <optional>
#include <vector>

struct UIRequests
{
	bool GoHome = false;
	bool Quit = false;
	int SceneIndex = -1;
	std::optional<glm::ivec2> Resize;
};

enum class MenuPage
{
	Hidden,
	Root,
	Chapters,
	Settings
};

class SceneRegistry;

class MenuUI
{
public:
	explicit MenuUI(const glm::ivec2& MaxResolution);

	void OnEscape();

	bool IsOpen() const { return m_Page != MenuPage::Hidden; }

	UIRequests Draw(const glm::ivec2& FramebufferSize, const SceneRegistry& Registry, int CurrentScene);

private:
	struct Resolution
	{
		const char* label;
		int Width;
		int Height;
	};
	MenuPage m_Page = MenuPage::Hidden;
	std::vector<Resolution> m_Resolutions;
	int m_SelectedResolution = -1;
	int m_SelectedChapter = 0;

	void Close() { m_Page = MenuPage::Hidden; }
	void DrawBackdrop();
	void DrawRoot(UIRequests& Requests, const glm::ivec2& FramebufferSize);
	void DrawChapters(UIRequests& Requests, const SceneRegistry& Registry, int CurrentScene);
	void DrawSettings(UIRequests& Requests, const glm::ivec2& FramebufferSize);
	void DrawChapterPicker(UIRequests& requests, const SceneRegistry& registry, int currentScene);
	void DrawHomePanel(UIRequests& requests, const SceneRegistry& registry, int currentScene);
};