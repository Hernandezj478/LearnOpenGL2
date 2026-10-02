#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <array>

class Window;

class Input
{
public:
	explicit Input(Window& window);
	~Input();
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	
	/*
	*! @brief Clears the per-frame values. Call once per frame before Window::PollEvents() 
	*/
	void NewFrame();

	void PostPoll();

	// Keyboard state
	bool IsDown(int Key) const;
	bool IsPressed(int Key) const;
	bool Released(int Key) const;

	bool MouseDown(int Button) const;
	bool MousePressed(int Button) const;

	/*
	*! @brief Mouse movement this frame in pixels. +x is right +y is up. Zero unless captured
	*/
	glm::vec2 MouseDelta() const { return m_MouseDelta; }
	/*
	*! @brief Scroll wheel movement this frame. Zero unless captured
	*/
	glm::vec2 ScrollDelta() const { return m_ScrollDelta; }

	void SetCursorCaptured(bool Captured);
	void ToggleCursorCaptured() { SetCursorCaptured(!m_Captured); }
	bool IsCursorCaptured() const { return m_Captured; }
private:
	// Called from handler window forwards GLFW events
	void OnKey(int Key, int Action);
	void OnMouseButton(int Button, int Action);
	void OnCursorPos(double X, double Y);
	void OnScroll(double DX, double DY);
	void OnFocus(bool Focused);

	void ClearHeldState();

	static bool ValidKey(int Key) { return Key >= 0 && Key <= GLFW_KEY_LAST; }
	static bool ValidButton(int Button) { return Button >= Button && Button <= GLFW_MOUSE_BUTTON_LAST; }

	Window& m_Window;

	std::array<bool, GLFW_KEY_LAST + 1> m_Down{};
	std::array<bool, GLFW_KEY_LAST + 1> m_Pressed{};
	std::array<bool, GLFW_KEY_LAST + 1> m_Released{};

	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_ButtonDown{};
	std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_ButtonPressed{};

	glm::vec2 m_MouseDelta{ 0.0f };
	glm::vec2 m_ScrollDelta{ 0.0f };
	glm::dvec2 m_LastCursor{ 0.0 };
	bool m_HasLastCursor = false;
	bool m_Captured = false;
	bool m_FocusChanged = false;
};