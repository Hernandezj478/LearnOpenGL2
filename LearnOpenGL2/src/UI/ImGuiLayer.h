#pragma once

class Window;
class ImGuiLayer
{
public:
	explicit ImGuiLayer(Window& window);
	~ImGuiLayer();

	ImGuiLayer(const ImGuiLayer&) = delete;
	ImGuiLayer& operator=(const ImGuiLayer&) = delete;

	/*
	*! @brief Stats a new ImGui frame. Call once per frame
	*/
	void BeginFrame();

	/*
	*! @brief Renders the ImGui draw data. Call once per frame after restoring framebuffer 0 and the viewport
	*/
	void EndFrame();

	bool WantsMouse() const;
};