#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <functional>
#include <utility>

class Window
{
public:
	Window();
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	/*
	*! @brief Function returns the flag from glfw if the window has terminated
	* @see glfwWindowShouldClose
	*/
	bool ShouldClose() const;
	/*
	*! @brief Called from menu context when the application should be closed out and sets the flag to close the window
	* @see glfwSetWindowShouldClose
	*/
	void RequestClose();
	/*
	*! @brief Wrapper to poll all glfw events
	* @see glfwPollEvents
	*/
	void PollEvents();
	/*
	*! @brief Function used while application is minimized
	* @see glfwWaitEvents
	*/
	void WaitEvents();
	/*
	*! @brief Wrapper function to call glfwSwapBuffers
	* @see glfwSwapBuffers
	*/
	void SwapBuffers();
	/*
	*! @brief Wrapper to call the current time from glfw
	* @return Time in double format
	* @see glfwGetTime
	*/
	double Time() const;
	/*
	*! @brief Gets the current framebuffer size (width, height), stores them in the window cache
	* @return Vec2: Width, Height
	*/
	glm::ivec2 FramebufferSize() const;
	/*
	*! @brief Resets bFrameBufferSizeChanged flag once the call to change the framebuffer size has finished. 
	*/
	bool ConsumeResize();
	/*
	*! @brief Set the current window size
	* @param[in] Width Window width
	* @param[in] Height Window height
	*/
	void SetWindowSize(int Width, int Height);
	/*
	*! @brief Wrapper function that sets window title
	* @param[in] Title String of window title
	* @see glfwSetWindowTitle()
	*/
	void SetWindowTitle(const char* Title);
	/*
	*! @brif Wrapper to get the current monitor display size
	* @return Width and Height
	*/
	glm::ivec2 MonitorSize();
	/*
	*! @brief Gives the raw handle to the glfw window
	* @return p_Window
	*/
	GLFWwindow* Handle() const;
	/*
	*! @brief Handler for keyboard inputs
	*/
	using KeyHandler = std::function<void(int Key, int Scancode, int Action, int Mods)>;
	/*
	*! @brif Handler for mouse inputs 
	*/
	using MouseButtonHandler = std::function<void(int Button, int Action, int Mods)>;
	using CursorHandler = std::function<void(double X, double Y)>;
	using ScrollHandler = std::function<void(double DX, double DY)>;
	using FocusHandler = std::function<void(bool Focused)>;


	void SetKeyHandler(KeyHandler Handler) { m_OnKey = std::move(Handler); }
	void SetMouseButtonHandler(MouseButtonHandler Handler) { m_OnMouseButton = std::move(Handler); }
	void SetCursorHandler(CursorHandler Handler) { m_OnCursor = std::move(Handler); }
	void SetScrollHandler(ScrollHandler Handler) { m_OnScroll = std::move(Handler); }
	void SetFocusHandler(FocusHandler Handler) { m_OnFocus = std::move(Handler); }

private:
	GLFWwindow* p_Window;
	bool bFramebufferSizeChanged;

	KeyHandler m_OnKey;
	MouseButtonHandler  m_OnMouseButton;
	CursorHandler m_OnCursor;
	ScrollHandler m_OnScroll;
	FocusHandler m_OnFocus;

	/*
	*! @brief This function sets the flag bFrameBufferSizeChanged when the framebuffer has changed since the last update.
	*/
	void OnFrameBufferResized();
};