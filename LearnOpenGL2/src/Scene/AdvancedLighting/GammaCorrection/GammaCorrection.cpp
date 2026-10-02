#include "GammaCorrection.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Shapes/Plane.h"

#include "Scene/SceneContext.h"
#include <imgui/imgui.h>

GammaCorrection::GammaCorrection(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Plane = std::make_unique<Plane>();
	m_FloorTexture = std::make_unique<Texture>("res/textures/wood.png");
	m_FloorTextureGamma = std::make_unique<Texture>("res/textures/wood.png");
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/GammaCorrection/GammaCorrection.glsl");
	m_Shader->Bind();
	m_Shader->SetUniform1i("floorTexture", 0);
	
	m_FloorTexture->SyncTexture();
	m_FloorTextureGamma->SetGammaCorrection(true);
	m_FloorTextureGamma->SyncTexture();

	m_LightPositions = {
		glm::vec3(-6.0f, 0.5f, 0.0f),
		glm::vec3(-2.0f, 0.5f, 0.0f),
		glm::vec3( 2.0f, 0.5f, 0.0f),
		glm::vec3( 6.0f, 0.5f, 0.0f)
	};

	m_LightColors = {
		glm::vec3(0.25f),
		glm::vec3(0.50f),
		glm::vec3(0.75f),
		glm::vec3(1.0f)
	};
}

void GammaCorrection::Render()
{
	m_Context.Renderer.Clear(DARK_GREY, COLOR_DEPTH);

	// Render Here
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 100.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f));
	m_Shader->SetUniformMat4f("model", model);

	m_Shader->SetUniform3fv("lightPositions", m_LightPositions.data(), 4);
	m_Shader->SetUniform3fv("lightColors", m_LightColors.data(), 4);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_Shader->SetUniform1i("bLinearGamma", m_UseLinearGamma);
	m_Shader->SetUniform1f("uvScale", m_UVScale);
	if (m_UseGamma)
	{
		m_FloorTextureGamma->Bind();
	}
	else
	{
		m_FloorTexture->Bind();
	}
	m_Plane->Draw(*m_Shader, m_Context.Renderer);
}

void GammaCorrection::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Gamma Correction", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Checkbox("Use Gamma", &m_UseGamma);
		ImGui::Checkbox("Use Linear Gamma Correction", &m_UseLinearGamma);
		ImGui::Separator();
		ImGui::Text("CTRL + LMB to manually enter value");
		ImGui::SliderFloat("UV Scale", &m_UVScale, 1.0f, 10.0f);
	}
	ImGui::End();
}