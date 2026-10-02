#include "MultipleLights.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/ColorPalette.h"

#include "Core/Input.h"
#include "Scene/SceneContext.h"

#include <iterator>
#include <string>

namespace
{
	glm::vec3 SunDirection(-0.2f, -1.0f, -0.3f);
	constexpr float dirLight_Ambient = 0.2f;
	constexpr float pointLight_Ambient = 0.5f;
	constexpr float maxStrength = 5.0f;
	const glm::vec3 pointLightPositions[] =
	{
		glm::vec3( 0.7f,  0.2f,  2.0f),
		glm::vec3( 2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f)
	};
	const char* sceneNames[] = {"Desert", "Factory", "Horror", "Biochemical Lab"};
}

MultipleLights::MultipleLights(const SceneContext& context) : Scene3D(context)
{
	m_LightShader = std::make_unique<Shader>("res/shaders/Lighting/MultiLights.glsl");
	m_DiffuseMap = std::make_unique<Texture>("res/textures/container2.png");
	m_SpecularMap = std::make_unique<Texture>("res/textures/container2_specular.png");
	m_Cube = std::make_unique<Cube>();
	
	m_DiffuseMap->SetFlipImage(false);
	m_SpecularMap->SetFlipImage(false);

	m_DiffuseMap->LoadPixels();
	m_SpecularMap->LoadPixels();

	m_DiffuseMap->Upload();
	m_SpecularMap->Upload();

	glEnable(GL_DEPTH_TEST);

	SetLightColors();
	SetLightFalloffValues();
	SetCubePositions(10, true);

	m_LightShader->Bind();
	m_LightShader->SetUniform1i("material.diffuse", 0);
	m_LightShader->SetUniform1i("material.specular", 1);
	m_LightShader->SetUniform1i("material.emission", 2);
	m_LightShader->SetUniform1f("material.shininess", 32.0f);
	m_LightShader->Unbind();
}

MultipleLights::~MultipleLights() = default;

void MultipleLights::Update(float deltaTime)
{
	Scene3D::Update(deltaTime);
	const Input& input = m_Context.Input;
	if (input.IsCursorCaptured() && input.IsPressed(GLFW_KEY_F))
	{
		bIsFlashlightOn = !bIsFlashlightOn;
	}
}

void MultipleLights::Render()
{
	m_Context.Renderer.Clear(m_ClearColors.at(m_SceneSelect), COLOR_DEPTH);
	if (m_LightsDirty)
	{
		UpdateLighting();
	}
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.f);

	m_LightShader->Bind();
	m_LightShader->SetUniformMat4f("projection", projection);
	m_LightShader->SetUniformMat4f("view", view);
	m_LightShader->SetUniformVec3("viewPos", m_Camera.GetPosition());

	m_LightShader->SetUniform1i("flashlightOn", bIsFlashlightOn);
	m_LightShader->SetUniformVec3("spotLight.position", m_Camera.GetPosition());
	m_LightShader->SetUniformVec3("spotLight.direction", m_Camera.GetFront());

	m_DiffuseMap->Bind(0);
	m_SpecularMap->Bind(1);
	for (const glm::vec3& position : m_CubePositions)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		m_LightShader->SetUniformMat4f("model", model);
		m_Cube->Draw(*m_LightShader, m_Context.Renderer);
	}
	m_DiffuseMap->Unbind();
	m_SpecularMap->Unbind();
	m_LightShader->Unbind();

	const std::vector<glm::vec4>& pointColors = m_PointLightColors.at(m_SceneSelect);
	for (int i = 0; i < 4; i++)
	{
		m_Marker.Draw(m_Context.Renderer, view, projection, pointLightPositions[i], glm::vec3(pointColors[i]), m_Brightness);
	}
}

void MultipleLights::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Multiple Lights", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		bool changed = false;
		if (ImGui::BeginCombo("Environment", sceneNames[m_SceneSelect]))
		{
			for (int i = 0; i < static_cast<int>(std::size(sceneNames)); i++)
			{
				const bool isSelected = (i == m_SceneSelect);
				if (ImGui::Selectable(sceneNames[i], isSelected))
				{
					m_SceneSelect = static_cast<SceneType>(i);
					changed = true;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		ImGui::Text("Light Casters");
		changed |= ImGui::Checkbox("Directional Light", &m_DirectionalOn);
		changed |= ImGui::Checkbox("Point Lights", &m_PointLightsOn);

		ImGui::Separator();

		if (ImGui::BeginCombo("Point light range", std::to_string(m_LightRange).c_str()))
		{
			for (const auto& entry : m_LightFalloffValues)
			{
				if (ImGui::Selectable(std::to_string(entry.first).c_str(), entry.first == m_LightRange))
				{
					m_LightRange = entry.first;
					changed = true;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SliderFloat("Marker brightness", &m_Brightness, 0.0, 20.0f);

		ImGui::Separator();
		ImGui::Text("Light Strength");
		changed |= ImGui::SliderFloat("Ambient", &m_AmbientStrength, 0.0f, maxStrength);
		changed |= ImGui::SliderFloat("Diffuse", &m_DiffuseStrength, 0.0f, maxStrength);
		changed |= ImGui::SliderFloat("Specular", &m_SpecularStrength, 0.0f, maxStrength);

		if (changed)
		{
			m_LightsDirty = true;
		}
	}
	ImGui::End();
}

void MultipleLights::SetLightColors()
{
	m_ClearColors[DESERT]	= DESERT_SKY;
	m_ClearColors[FACTORY]	= NEAR_BLACK;
	m_ClearColors[HORROR]	= BLACK;
	m_ClearColors[LAB]		= LIGHT_GREY;

	m_DirectionalColors[DESERT]		= DESERT_SUN;
	m_DirectionalColors[FACTORY]	= INDIGO;
	m_DirectionalColors[HORROR]		= VERY_DARK_GREY;
	m_DirectionalColors[LAB]		= WHITE;

	m_PointLightColors[DESERT]	= { ORANGE, RED, YELLOW, BRIGHT_BLUE };
	m_PointLightColors[FACTORY] = { MUTED_BLUE, SLATE_BLUE, NAVY, GREY };
	m_PointLightColors[HORROR]	= { NEAR_BLACK, NEAR_BLACK, NEAR_BLACK, BLOOD_RED };
	m_PointLightColors[LAB]		= { ACID_GREEN, ACID_GREEN, ACID_GREEN, ACID_GREEN };

	m_SpotlightColors[DESERT]	= DARK_YELLOW;
	m_SpotlightColors[FACTORY]	= WHITE;
	m_SpotlightColors[HORROR]	= WHITE;
	m_SpotlightColors[LAB]		= GREEN;
}

void MultipleLights::SetLightFalloffValues()
{
	m_LightFalloffValues[7] = glm::vec3(1.0, 0.7, 1.8);
	m_LightFalloffValues[13] = glm::vec3(1.0, 0.35, 0.44);
	m_LightFalloffValues[20] = glm::vec3(1.0, 0.22, 0.20);
	m_LightFalloffValues[32] = glm::vec3(1.0, 0.14, 0.07);
	m_LightFalloffValues[50] = glm::vec3(1.0, 0.09, 0.032);
	m_LightFalloffValues[65] = glm::vec3(1.0, 0.07, 0.017);
	m_LightFalloffValues[100] = glm::vec3(1.0, 0.045, 0.0075);
	m_LightFalloffValues[160] = glm::vec3(1.0, 0.027, 0.0028);
	m_LightFalloffValues[200] = glm::vec3(1.0, 0.022, 0.0019);
	m_LightFalloffValues[325] = glm::vec3(1.0, 0.014, 0.0007);
	m_LightFalloffValues[600] = glm::vec3(1.0, 0.007, 0.0002);
	m_LightFalloffValues[3250] = glm::vec3(1.0, 0.0014, 0.000007);
}

void MultipleLights::SetCubePositions(int numCubes, bool randomPos)
{
	m_CubePositions.clear();
	float offset = 0.0f;
	for (int i = 0; i < numCubes; i++)
	{
		float cubeCenterRadius = 0.5f;
		if (!randomPos)
		{
			offset += cubeCenterRadius;
			m_CubePositions.push_back(glm::vec3(offset, cubeCenterRadius, i < 5 ? -0.5f : -1.5f));
			offset += cubeCenterRadius;
			if (i == 4)
			{
				offset = 0.0f;
			}
		}
		else
		{
			m_CubePositions.push_back(glm::vec3(rand() % 7 - 3, rand() % 8 - 2, rand() % 16 - 15));
		}
	}
}

void MultipleLights::UpdateLighting()
{
	const glm::vec3& falloff = m_LightFalloffValues[m_LightRange];
	const glm::vec3 sun = glm::vec3(m_DirectionalColors.at(m_SceneSelect)) * (m_DirectionalOn ? 1.0f : 0.0f);
	m_LightShader->Bind();
	m_LightShader->SetUniformVec3("dirLight.direction", SunDirection);
	m_LightShader->SetUniformVec3("dirLight.ambient", sun * dirLight_Ambient * m_AmbientStrength);
	m_LightShader->SetUniformVec3("dirLight.diffuse", sun * m_DiffuseStrength);
	m_LightShader->SetUniformVec3("dirLight.specular", sun * m_SpecularStrength);

	const std::vector<glm::vec4>& pointColors = m_PointLightColors.at(m_SceneSelect);
	const float pointOn = m_PointLightsOn ? 1.0f : 0.0f;
	for (int i = 0; i < 4; i++)
	{
		const std::string base = "pointLights[" + std::to_string(i) + "].";
		const glm::vec3 color = glm::vec3(pointColors[i]) * pointOn;
		m_LightShader->SetUniformVec3(base + "position", pointLightPositions[i]);
		m_LightShader->SetUniformVec3(base + "ambient", color * pointLight_Ambient * m_AmbientStrength);
		m_LightShader->SetUniformVec3(base + "diffuse", color * m_DiffuseStrength);
		m_LightShader->SetUniformVec3(base + "specular", color * m_SpecularStrength);
		m_LightShader->SetUniform1f(base + "constant", falloff.x);
		m_LightShader->SetUniform1f(base + "linear", falloff.y);
		m_LightShader->SetUniform1f(base + "quadratic", falloff.z);
	}
	const glm::vec3 spotLight = glm::vec3(m_SpotlightColors.at(m_SceneSelect));
	m_LightShader->SetUniform1f("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
	m_LightShader->SetUniform1f("spotLight.outerCutoff", glm::cos(glm::radians(17.5f)));
	m_LightShader->SetUniform3f("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_LightShader->SetUniformVec3("spotLight.diffuse", spotLight * m_DiffuseStrength);
	m_LightShader->SetUniformVec3("spotLight.specular", spotLight * m_SpecularStrength);
	m_LightShader->SetUniform1f("spotLight.constant", falloff.x);
	m_LightShader->SetUniform1f("spotLight.linear", falloff.y);
	m_LightShader->SetUniform1f("spotLight.quadratic", falloff.z);

	m_LightShader->Unbind();;
	m_LightsDirty = false;
}
