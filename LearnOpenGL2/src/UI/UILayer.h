#pragma once
#include <glm/glm.hpp>

#include <optional>
#include <vector>

class SceneRegistry;
class SceneManager;

struct UIRequests
{
	bool GoHome = false;
	bool Quit = false;
	int SceneIndex = -1;
	std::optional<glm::ivec2> Resize;
};

class UILayer
{
public:
	explicit UILayer(const glm::ivec2& MaxResolution);

	UIRequests Draw(const SceneRegistry& Registry, SceneManager& SceneManager, 
		const glm::ivec2& FrameBufferSize, int FPS, double timeElapsed);

private:
	struct Resolution
	{
		const char* Label;
		int Width;
		int Height;
	};

	UIRequests DrawMenuBar(const SceneRegistry& Registry, SceneManager& SceneManager);
	void DrawDataOverlay(int fps, double timeElapsed);
	void DrawScenePanael(SceneManager& SceneManager);

	std::vector<Resolution> m_Resolutions;
	bool m_FirstTime = true;
	unsigned int m_DockspaceID = 0;
};