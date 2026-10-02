#include "Window.h"

#include <stdexcept>

Window::Window()
{
	// GLFW setup
	if (!glfwInit())
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Initialize window variables
	bFramebufferSizeChanged = true;
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	// Create the window
	p_Window = glfwCreateWindow(mode->width, mode->height, "OpenGL Showcase", NULL, NULL);
	if(p_Window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW Window");
	}
	glfwMakeContextCurrent(p_Window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLAD");
	}
	glfwSwapInterval(1);

	glfwSetWindowUserPointer(p_Window, this);
	glfwSetFramebufferSizeCallback(p_Window, [](GLFWwindow* w, int, int) {
		static_cast<Window*>(glfwGetWindowUserPointer(w))->OnFrameBufferResized();
		});
	glfwSetKeyCallback(p_Window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
		if (self->m_OnKey) self->m_OnKey(key, scancode, action, mods);
		});
	glfwSetMouseButtonCallback(p_Window, [](GLFWwindow* w, int button, int action, int mods) {
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
		if (self->m_OnMouseButton) self->m_OnMouseButton(button, action, mods);
		});
	glfwSetCursorPosCallback(p_Window, [](GLFWwindow* w, double x, double y) {
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
		if (self->m_OnCursor) self->m_OnCursor(x, y);
		});
	glfwSetScrollCallback(p_Window, [](GLFWwindow* w, double dx, double dy) {
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
		if (self->m_OnScroll) self->m_OnScroll(dx, dy);
		});
	glfwSetWindowFocusCallback(p_Window, [](GLFWwindow* w, int focused) {
		auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
		if (self->m_OnFocus) self->m_OnFocus(focused);
		});
}

Window::~Window()
{
	glfwDestroyWindow(p_Window);
	glfwTerminate();
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(p_Window);
}

void Window::RequestClose()
{
	glfwSetWindowShouldClose(p_Window, GLFW_TRUE);
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::WaitEvents()
{
	glfwWaitEvents();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(p_Window);
}

double Window::Time() const
{
	return glfwGetTime();
}

glm::ivec2 Window::FramebufferSize() const
{
	int w = 0, h = 0;
	glfwGetFramebufferSize(p_Window, &w, &h);
	return { w, h };
}

bool Window::ConsumeResize()
{
	const bool changed = bFramebufferSizeChanged;
	bFramebufferSizeChanged = false;
	return changed;
}

void Window::SetWindowSize(int Width, int Height)
{
	glfwSetWindowSize(p_Window, Width, Height);
}

void Window::SetWindowTitle(const char* Title)
{
	glfwSetWindowTitle(p_Window, Title);
}

glm::ivec2 Window::MonitorSize()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	return { mode->width, mode->height};
}

GLFWwindow* Window::Handle() const
{
	return p_Window;
}

void Window::OnFrameBufferResized()
{
	bFramebufferSizeChanged = true;
}