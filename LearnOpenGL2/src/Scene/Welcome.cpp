#include "Welcome.h"

#include "SceneContext.h"
#include "Graphics/Renderer.h"
#include "Graphics/ColorPalette.h"

#include <imgui/imgui.h>

void WelcomeScene::Render()
{
	m_Context.Renderer.Clear(GREY);
}

void WelcomeScene::OnGui()
{
	const ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f - 12.0f, io.DisplaySize.y * 0.5f),
		ImGuiCond_Always, ImVec2(1.0f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(480.0f, 0.0f));

	const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoSavedSettings;

	if (ImGui::Begin("Welcome", nullptr, flags))
	{
		ImGui::TextWrapped("OpenGL Showcase brings the sections of the Learn OpenGL course all together in one application!"
		" This wouldnt be possible without the great work of Joey DeVries!");
		ImGui::Spacing();
		ImGui::TextWrapped("Press ESC to open the menu, then pick a chapter and a section");

		ImGui::Separator();
		ImGui::Text("Controls");
		ImGui::BulletText("ESC: Open or close the menu");
		ImGui::BulletText("Hold RMB: Fly camera in 3D scene");
		ImGui::BulletText("W/S/A/D: Move camera ");
		ImGui::BulletText("Q/E: Move up and down");
		ImGui::BulletText("Scoll wheel (while flying): change camera fly speed");
		ImGui::BulletText("Keypad +/-/ENTER: adjust or reset camera FOV");
		ImGui::BulletText("P: Toggle FPS camera");

		ImGui::Separator();
		ImGui::Text("Credit");
		ImGui::TextWrapped(
			"Baed on the Learn OpenGL course by Joey DeVries (learnopengl.com)"
			"Built with GLFW, GLAD, GLM and Dear ImGui.");
	}
	ImGui::End();
}
