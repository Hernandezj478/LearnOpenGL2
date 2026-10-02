#include "Materials.h"

#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/ColorPalette.h"

#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

Materials::Materials(const SceneContext& context) : Scene3D(context)
{
	m_Shader = std::make_unique<Shader>("res/shaders/Lighting/Materials.glsl");
	m_Cube = std::make_unique<Cube>();

	glEnable(GL_DEPTH_TEST);
	m_Material.ambient = ORANGE;
	m_Material.diffuse = ORANGE;
	m_Material.specular = GREY;
	m_Material.shininess = 32;

	lightColor = WHITE;
	m_LightAmbient = glm::vec3(0.2f);
	m_LightDiffuse = glm::vec3(0.5f);
	m_LightSpecular = glm::vec3(1.0f);

	m_LightPosition = glm::vec3(1.2f, 1.0f, 2.0f);

	m_MaterialSelection = "cyan plastic";
}

Materials::~Materials() = default;

void Materials::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(m_Camera.GetFOV(), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("model", model);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	
	if (bUseMaterial)
	{
		m_Material.ambient = MaterialLibrary.at(m_MaterialSelection).ambient;
		m_Material.diffuse = MaterialLibrary.at(m_MaterialSelection).diffuse;
		m_Material.specular = MaterialLibrary.at(m_MaterialSelection).specular;
		m_Material.shininess = MaterialLibrary.at(m_MaterialSelection).shininess * 128.0f;

		m_LightAmbient = glm::vec3(1.0f);
		m_LightDiffuse = glm::vec3(1.0f);
	}
	else
	{
		m_Material.ambient = ORANGE;
		m_Material.diffuse = ORANGE;
		m_Material.specular = GREY;
		m_Material.shininess = 32;
	}

	if (bUsePartyLights)
	{

		lightColor.r = glm::abs(static_cast<float>(cos(glfwGetTime() * 2.0)));
		lightColor.g = glm::abs(static_cast<float>(cos(glfwGetTime() * 0.7)));
		lightColor.b = glm::abs(static_cast<float>(cos(glfwGetTime() * 1.3)));

		m_LightDiffuse = lightColor * glm::vec3(0.5f);
		m_LightAmbient = m_LightDiffuse * glm::vec3(0.2f);
	}
	else
	{
		lightColor = WHITE;
		m_LightDiffuse = glm::vec3(0.5f);
		m_LightAmbient = glm::vec3(0.2f);
	}
	m_Shader->SetUniformVec3("material.ambient", m_Material.ambient);
	m_Shader->SetUniformVec3("material.diffuse", m_Material.diffuse);
	m_Shader->SetUniformVec3("material.specular", m_Material.specular);
	m_Shader->SetUniform1f("material.shininess", m_Material.shininess);

	m_Shader->SetUniformVec3("light.position", m_LightPosition);
	m_Shader->SetUniformVec3("light.ambient", m_LightAmbient);
	m_Shader->SetUniformVec3("light.diffuse", m_LightDiffuse);
	m_Shader->SetUniformVec3("light.specular", m_LightSpecular);

	m_Cube->Draw(*m_Shader, m_Context.Renderer);

	m_LightMarker.Draw(m_Context.Renderer, view, projection, m_LightPosition, lightColor);
}

void Materials::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Materials", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::BeginDisabled(bUseMaterial);
		ImGui::Checkbox("Party Lights", &bUsePartyLights);
		ImGui::EndDisabled();
		ImGui::Separator();
		ImGui::Checkbox("Change cube material", &bUseMaterial);
		if (bUseMaterial)
		{
			bUsePartyLights = false;
		}
		ImGui::TextDisabled("Material List");
		if (ImGui::BeginCombo("##Material", m_MaterialSelection.c_str()))
		{
			for (auto& material : MaterialLibrary)
			{
				if (ImGui::Selectable(material.first.c_str(), material.first == m_MaterialSelection))
				{
					m_MaterialSelection = material.first;
					m_Material = material.second;
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::End();
}
