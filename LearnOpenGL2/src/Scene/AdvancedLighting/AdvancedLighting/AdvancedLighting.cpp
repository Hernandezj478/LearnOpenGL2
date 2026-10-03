#include "AdvancedLighting.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"
#include "Graphics/Shapes/Plane.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

AdvancedLighting::AdvancedLighting(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Light = std::make_unique<LightMarker>();

	m_Floor = std::make_unique<Plane>();
	m_FloorTexture = std::make_unique<Texture>("res/textures/wood.png");
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/AdvancedLighting/AdvancedLighting.glsl");
	m_Shader->Bind();
	m_Shader->SetUniform1i("floorTexture", 0);
	m_Shader->SetUniform1f("light.cutoff", glm::cos(glm::radians(65.0f)));
	m_Shader->SetUniform1f("light.outerCutoff", glm::cos(glm::radians(80.0f)));
	m_Shader->SetUniformVec3("light.ambient", glm::vec3(0.05f));
	m_Shader->SetUniformVec3("light.specular", glm::vec3(0.3f));
	m_Shader->SetUniform1f("light.constant", 1.0f);
	m_Shader->SetUniform1f("light.linear", 0.09f);
	m_Shader->SetUniform1f("light.quadratic", 0.032f);

	m_FloorTexture->SyncTexture();

	m_LightPosition = glm::vec3(0.0f, 0.25f, 0.0f);
	m_LightTypeMap[POINT] = "Point Light";
	m_LightTypeMap[SPOT] = "Spot Light";
}

void AdvancedLighting::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 100.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_Shader->SetUniform1i("bBlinnPhong", bBlinnPhong);
	m_Shader->SetUniform1i("lightType", m_LightSelection);

	switch (m_LightSelection)
	{
	case POINT:
		m_Shader->SetUniformVec3("light.position", m_LightPosition);
		break;
	case SPOT:
		m_Shader->SetUniformVec3("light.direction", glm::vec3(0.0, -0.5, 1.0));
		break;
	}
	m_FloorTexture->Bind();
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	m_Shader->SetUniformMat4f("model", model);
	m_Floor->Draw(*m_Shader, m_Context.Renderer);

	m_Light->Draw(m_Context.Renderer, view, projection, m_LightPosition, WHITE, 1.0f, 0.05f);
}

void AdvancedLighting::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Advanced Lighting", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Checkbox("Blinn-Phong", &bBlinnPhong);
		if (ImGui::BeginCombo("Light Type", m_LightTypeMap[m_LightSelection].c_str()))
		{
			for (auto light : m_LightTypeMap)
			{
				bool selected = (light.first == m_LightSelection);
				if (ImGui::Selectable(light.second.c_str(), selected))
				{
					m_LightSelection = light.first;
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::End();
}
