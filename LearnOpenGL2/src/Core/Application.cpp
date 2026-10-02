#include "Application.h"
#include <algorithm>

#include "Graphics/ColorPalette.h"

Application::Application() : 
	m_Input(m_Window), m_ImGui(m_Window), m_Menu(m_Window.MonitorSize()),
	m_Context{m_Window, m_Input, m_Renderer, std::filesystem::current_path() / "res"}, 
	m_SceneManager(m_Registry, m_Context)
{}

Application::~Application()
{}

void Application::Run()
{
	m_LastFrame = m_Window.Time();
	while (!m_Window.ShouldClose())
	{
		if (!BeginFrame())
		{
			continue;
		}
		if (m_Input.IsPressed(GLFW_KEY_ESCAPE))
		{
			m_Menu.OnEscape();
		}
		UpdateCursorMode();

		m_SceneManager.Update(m_DeltaTime);
		m_SceneManager.Render();
		DrawUI();
		if (!m_Menu.IsOpen())
		{
			m_SceneManager.OnGui();
		}
		EndFrame();
	}
}

bool Application::BeginFrame()
{
	ApplyPendingRequest();
	m_Input.NewFrame();
	m_Window.PollEvents();
	m_Input.PostPoll();
	const double now = m_Window.Time();
	m_DeltaTime = static_cast<float>(std::min(now - m_LastFrame, 0.1));
	m_LastFrame = now;
	FrameSize = m_Window.FramebufferSize();
	// If window is minimized, wait for events and not process them.
	if (FrameSize.x == 0 || FrameSize.y == 0)
	{
		m_Window.WaitEvents();
		return false;
	}
	// If window has been resized, consume resize to set up next frame
	if (m_Window.ConsumeResize())
	{
		glViewport(0, 0, FrameSize.x, FrameSize.y);
		// Set scene camera to update with new window size
		m_SceneManager.OnResize(FrameSize.x, FrameSize.y);
	}
	// Start the ImGui frame
	m_ImGui.BeginFrame();
	return true;
}

void Application::ApplyPendingRequest()
{
	// If any resize pending, set the window size and apply on frame
	if (m_PendingResize)
	{
		m_Window.SetWindowSize(m_PendingResize->x, m_PendingResize->y);
		m_PendingResize.reset();
	}
	// If any scene change has been requested, process
	if (m_SceneManager.ApplyPending())
	{
		m_Window.SetWindowTitle(m_SceneManager.CurrentName().c_str());
	}
}

void Application::DrawUI()
{
	const UIRequests requests = m_Menu.Draw(m_Window.FramebufferSize(), m_Registry, m_SceneManager.CurrentIndex());
	if (requests.Quit)
	{
		m_Window.RequestClose();
	}
	if (requests.Resize)
	{
		m_PendingResize = requests.Resize;
	}
	if (requests.GoHome)
	{
		m_SceneManager.RequestHome();
	}
	if (requests.SceneIndex >= 0)
	{
		m_SceneManager.RequestScene(requests.SceneIndex);
	}
}

void Application::EndFrame()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, FrameSize.x, FrameSize.y);
	m_ImGui.EndFrame();
	m_Window.SwapBuffers();
}

void Application::UpdateCursorMode()
{
	if (m_Menu.IsOpen())
	{
		m_Input.SetCursorCaptured(false);
		return;
	}
	if (m_Input.IsCursorCaptured())
	{
		if (!m_Input.MouseDown(GLFW_MOUSE_BUTTON_RIGHT))
		{
			m_Input.SetCursorCaptured(false);
		}
	}
	else if (m_Input.MousePressed(GLFW_MOUSE_BUTTON_RIGHT) && !m_ImGui.WantsMouse())
	{
		m_Input.SetCursorCaptured(true);
	}
}
