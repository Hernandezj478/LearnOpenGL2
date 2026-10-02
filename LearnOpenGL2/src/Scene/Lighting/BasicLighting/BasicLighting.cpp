#include "BasicLighting.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Renderer.h"

#include "Graphics/ColorPalette.h"
#include "Scene/SceneContext.h"

BasicLighting::BasicLighting(const SceneContext& context) : Scene3D(context)
{
	m_Shader = std::make_unique<Shader>("res/shaders/Lighting/BasicLighting.glsl");
	m_Cube = std::make_unique<Cube>();
	lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
	SetShininessValues();
	glEnable(GL_DEPTH_TEST);

}

BasicLighting::~BasicLighting() = default;

void BasicLighting::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	if (m_LightsDirty)
	{
		UpdateLighting();
	}

	glm::mat4 model(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(m_Camera.GetFOV(), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("model", model);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());

	m_Cube->Draw(*m_Shader, m_Context.Renderer);
	m_LightMarker.Draw(m_Context.Renderer, view, projection, lightPosition, WHITE);
}

void BasicLighting::OnGui()
{
	bool changed = false;
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Basic Lights", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		changed |= ImGui::SliderFloat("Ambient Strength", &m_AmbientStrength, 0.1f, 1.0f);
		changed |= ImGui::SliderFloat("Specular Strength", &m_SpecularStrength, 0.1f, 1.0f);
		if (ImGui::BeginCombo("Shininess", std::to_string(m_Shininess).c_str()))
		{
			for (int i = 0; i < m_ShininessValues.size(); i++)
			{
				if (ImGui::Selectable(std::to_string(m_ShininessValues[i]).c_str(), m_ShininessValues[i] == m_Shininess))
				{
					m_Shininess = m_ShininessValues[i];
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		if (changed)
		{
			m_LightsDirty = true;
		}
	}
	ImGui::End();
}

void BasicLighting::UpdateLighting()
{
	m_Shader->Bind();
	m_Shader->SetUniform1f("ambientStrength", m_AmbientStrength);
	m_Shader->SetUniform1f("specularStrength", m_SpecularStrength);
	m_Shader->SetUniform1i("shininess", m_Shininess);
	m_Shader->SetUniformVec3("lightColor", WHITE);
	m_Shader->SetUniformVec3("objectColor", ORANGE);
	m_Shader->SetUniformVec3("lightPos", lightPosition);
	m_Shader->Unbind();

	m_LightsDirty = false;
}

void BasicLighting::SetShininessValues()
{
	for (int i = 2; i <= 256;)
	{
		m_ShininessValues.push_back(i);
		i = i << 1;
	}
}
