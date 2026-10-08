#pragma once

#include "Window.h"
#include "Input.h"
#include "UI/ImGuiLayer.h"
#include "UI/MenuUI.h"
#include "Graphics/Renderer.h"
#include "Scene/SceneRegistry.h"
#include "Scene/SceneContext.h"
#include "Scene/SceneManager.h"

#include <optional>

class Application
{
public:
	Application();
	~Application();

	void Run();
	SceneRegistry& Registry() { return m_Registry; }
private:
	Window m_Window;
	Input m_Input;
	ImGuiLayer m_ImGui;
	MenuUI m_Menu;
	Renderer m_Renderer;
	SceneRegistry m_Registry;
	SceneContext m_Context;
	SceneManager m_SceneManager;
	/*
	AssetCache m_Assets;
	*/

	double m_LastFrame = 0.0;
	float m_DeltaTime = 0.0f;
	int m_FrameCount = 0;
	float m_FPSTimer = 0.0f;
	int m_FPS = 0;

	glm::ivec2 FrameSize{0.0f, 0.0f};
	std::optional<glm::ivec2> m_PendingResize;

	/*
	*! @brief Poll events, calculate delta time, resize check, poll input, start ImGui frame
	*/
	bool BeginFrame();
	/*
	*! @brief Apply resize request, swap scenes, start next frame on scene switch
	*/
	void ApplyPendingRequest();
	void DrawUI();	// menu + scene panel, collects UI request bundle
	void EndFrame();	// restore FBO + viewport, render ImGUI, swap
	void UpdateCursorMode();
};