#include "LightCasters.h"

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Renderer.h"
#include "Graphics/ColorPalette.h"
#include "Graphics/Shapes/Cube.h"

#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

LightCasters::LightCasters(const SceneContext& context) : Scene3D(context)
{
	m_ShaderPtr = nullptr;

	m_DirectionalLight = std::make_shared<Shader>("res/shaders/Lighting/LightCasters/DirectionalLight.glsl");
	m_PointLight = std::make_shared<Shader>("res/shaders/Lighting/LightCasters/Pointlight.glsl");
	m_SpotLight = std::make_shared<Shader>("res/shaders/Lighting/LightCasters/Spotlight.glsl");

	m_DiffuseTexture = std::make_unique<Texture>("res/textures/container2.png");
	m_SpecularTexture = std::make_unique<Texture>("res/textures/container2_specular.png");
	m_Cube = std::make_unique<Cube>();

	m_DiffuseTexture->SetFlipImage(false);
	m_SpecularTexture->SetFlipImage(false);

	m_DiffuseTexture->LoadPixels();
	m_SpecularTexture->LoadPixels();

	m_DiffuseTexture->Upload();
	m_SpecularTexture->Upload();

	m_LightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
	m_PointlightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
	m_SpotlightPosition = m_Camera.GetPosition();

	glEnable(GL_DEPTH_TEST);

	m_DirectionalLight->Bind();
	m_DirectionalLight->SetUniform1i("material.diffuse", 0);
	m_DirectionalLight->SetUniform1i("material.specular", 1);
	m_DirectionalLight->SetUniform1f("material.shininess", 32.0);
	m_DirectionalLight->Unbind();

	m_PointLight->Bind();
	m_PointLight->SetUniform1i("material.diffuse", 0);
	m_PointLight->SetUniform1i("material.specular", 1);
	m_PointLight->SetUniform1f("material.shininess", 32.0);
	m_PointLight->Unbind();

	m_SpotLight->Bind();
	m_SpotLight->SetUniform1i("material.diffuse", 0);
	m_SpotLight->SetUniform1i("material.specular", 1);
	m_SpotLight->SetUniform1f("material.shininess", 32.0);
	m_SpotLight->SetUniform1f("light.cutoff", glm::cos(glm::radians(12.5f)));
	m_SpotLight->SetUniform1f("light.outerCutoff", glm::cos(glm::radians(17.5f)));
	m_SpotLight->Unbind();

	m_CubePositions.push_back(glm::vec3( 0.0f,  0.0f,  0.0f));
	m_CubePositions.push_back(glm::vec3( 2.0f,  5.0f, -15.0f));
	m_CubePositions.push_back(glm::vec3(-1.5f, -2.2f, -2.5f));
	m_CubePositions.push_back(glm::vec3(-3.8f, -2.0f, -12.3f));
	m_CubePositions.push_back(glm::vec3( 2.4f, -0.4f, -3.5f));
	m_CubePositions.push_back(glm::vec3(-1.7f,  3.0f, -7.5f));
	m_CubePositions.push_back(glm::vec3( 1.3f, -2.0f, -2.5f));
	m_CubePositions.push_back(glm::vec3( 1.5f,  2.0f, -2.5f));
	m_CubePositions.push_back(glm::vec3( 1.5f,  0.2f, -1.5f));
	m_CubePositions.push_back(glm::vec3(-1.3f,  1.0f, -1.5f));

}

LightCasters::~LightCasters() = default;

void LightCasters::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(m_Camera.GetFOV(), m_Camera.GetAspectRatio(), 0.1f, 1000.f);

	if (bLightDirty)
	{
		UpdateLighting();
	}

	m_ShaderPtr->Bind();
	m_ShaderPtr->SetUniformMat4f("view", view);
	m_ShaderPtr->SetUniformMat4f("projection", projection);
	m_ShaderPtr->SetUniformVec3("viewPos", m_Camera.GetPosition());

	if (m_LightSelection == SPOT)
	{
		m_ShaderPtr->SetUniformVec3("light.position", m_Camera.GetPosition());
		m_SpotLight->SetUniformVec3("light.direction", m_Camera.GetFront());
	}

	m_DiffuseTexture->Bind(0);
	m_SpecularTexture->Bind(1);

	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_CubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_ShaderPtr->SetUniformMat4f("model", model);
		m_Cube->Draw(*m_ShaderPtr, m_Context.Renderer);
	}
	if (m_LightSelection == POINT)
	{
		m_LightMarker.Draw(m_Context.Renderer, view, projection, m_PointlightPosition, WHITE);
	}
	m_ShaderPtr->Unbind();
}

void LightCasters::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Light Casters", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::string selected = (m_LightSelection == DIRECTIONAL) ? "Directional"
			: (m_LightSelection == POINT) ? "Point" : "Spot";

		ImGui::Text("Light Selected: %s", selected.c_str());
		ImGui::Separator();
		if (ImGui::Button("Directional Light"))
		{
			m_LightSelection = DIRECTIONAL;
			bLightDirty = true;
		}
		if (ImGui::Button("Point Light"))
		{
			m_LightSelection = POINT;
			bLightDirty = true;
		}
		if (ImGui::Button("Spot Light"))
		{
			m_LightSelection = SPOT;
			bLightDirty = true;
		}
	}
	ImGui::End();
}

void LightCasters::LoadDirectionalLights()
{
	m_DirectionalLight->Bind();
	m_DirectionalLight->SetUniformVec3("light.direction", m_LightDirection);
	m_DirectionalLight->SetUniformVec3("light.ambient", DARK_GREY);
	m_DirectionalLight->SetUniformVec3("light.diffuse", GREY);
	m_DirectionalLight->SetUniformVec3("light.specular", WHITE);
	m_DirectionalLight->Unbind();
	m_ShaderPtr = m_DirectionalLight;
}

void LightCasters::LoadPointlights()
{
	m_PointLight->Bind();
	m_PointLight->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_PointLight->SetUniformVec3("light.position", m_PointlightPosition);
	m_PointLight->SetUniformVec3("light.ambient", DARK_GREY);
	m_PointLight->SetUniformVec3("light.diffuse", GREY);
	m_PointLight->SetUniformVec3("light.specular", WHITE);
	m_PointLight->SetUniform1f("light.constant", 1.0f);
	m_PointLight->SetUniform1f("light.linear", 0.09f);
	m_PointLight->SetUniform1f("light.quadratic", 0.032f);
	m_PointLight->Unbind();
	m_ShaderPtr = m_PointLight;
}

void LightCasters::LoadSpotlights()
{
	m_SpotLight->Bind();
	m_SpotLight->SetUniformVec3("light.ambient", GREY);
	m_SpotLight->SetUniformVec3("light.diffuse", GREY);
	m_SpotLight->SetUniformVec3("light.specular", WHITE);
	m_SpotLight->SetUniform1f("light.constant", 1.0f);
	m_SpotLight->SetUniform1f("light.linear", 0.09f);
	m_SpotLight->SetUniform1f("light.quadratic", 0.032f);
	m_SpotLight->Unbind();
	m_ShaderPtr = m_SpotLight;
}

void LightCasters::UpdateLighting()
{
	switch (m_LightSelection)
	{
	case DIRECTIONAL:
		LoadDirectionalLights();
		break;
	case POINT:
		LoadPointlights();
		break;
	case SPOT:
		LoadSpotlights();
		break;
	default:
		break;
	}
	bLightDirty = false;
}
