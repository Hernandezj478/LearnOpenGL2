#include "PBRLighting.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Shapes/Sphere.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

PBRLighting::PBRLighting(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);

	m_ModelLoader.Request(
		[]() -> std::unique_ptr<Model>
		{
			return std::make_unique<Model>("res/meshes/Backpack/Backpack.gltf");
		});
	m_Sphere = std::make_unique<Sphere>();
	m_Light = std::make_unique<LightMarker>();

	m_PBRLighting = std::make_unique<Shader>("res/shaders/PBR/Lighting/PBRLighting.glsl");
	m_PBRTexturedLighting = std::make_unique<Shader>("res/shaders/PBR/Lighting/PBRTexture.glsl");
	m_PBRModelLighting = std::make_unique<Shader>("res/shaders/PBR/Lighting/PBRModel.glsl");

	m_Albedo = std::make_unique<Texture>("res/textures/PBR/Rusted_Iron/albedo.png");
	m_Normal = std::make_unique<Texture>("res/textures/PBR/Rusted_Iron/normal.png");
	m_Metallic = std::make_unique<Texture>("res/textures/PBR/Rusted_Iron/metallic.png");
	m_Roughness = std::make_unique<Texture>("res/textures/PBR/Rusted_Iron/roughness.png");
	m_AO = std::make_unique<Texture>("res/textures/PBR/Rusted_Iron/ao.png");

	m_Albedo->SyncTexture();
	m_Normal->SyncTexture();
	m_Metallic->SyncTexture();
	m_Roughness->SyncTexture();
	m_AO->SyncTexture();

	m_LightData.push_back({ glm::vec3(-10.0f,  10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3( 10.0f,  10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3( 10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });

	m_PBRLighting->Bind();
	m_PBRLighting->SetUniformVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
	m_PBRLighting->SetUniform1f("ao", 1.0f);

	m_PBRTexturedLighting->Bind();
	m_PBRTexturedLighting->SetUniform1i("albedoMap", 0);
	m_PBRTexturedLighting->SetUniform1i("normalMap", 1);
	m_PBRTexturedLighting->SetUniform1i("metallicMap", 2);
	m_PBRTexturedLighting->SetUniform1i("roughnessMap", 3);
	m_PBRTexturedLighting->SetUniform1i("aoMap", 4);

	m_SceneSelection[BASIC] = "Basic Scene";
	m_SceneSelection[TEXTURE] = "Textured Scene";
	m_SceneSelection[MODEL] = "Model Scene";
}

void PBRLighting::Render()
{
	//m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);
	switch (m_SelectedScene)
	{
	case BASIC:
		BasicPBR();
		break;
	case TEXTURE:
		TexturedPBR();
		break;
	case MODEL:
		ModelPBR();
		break;
	}
}

void PBRLighting::Update(float deltaTime)
{
	Scene3D::Update(deltaTime);

	m_ModelLoader.Update(
		[this](std::unique_ptr<Model> model)
		{
			model->Finalize();
			m_Backpack = std::move(model);
		},
		[this](const std::exception& e)
		{
			m_LastError = e.what();
		}
	);
	if (!m_LastError.empty())
	{
		std::string msg = m_LastError;
		m_LastError.clear();
		throw std::runtime_error(msg);
	}
}

void PBRLighting::OnGui()
{
	ImGui::Checkbox("Gamma Correction", &m_UseGamma);
	ImGui::Separator();
	if (ImGui::BeginCombo("Scene Selection", m_SceneSelection[m_SelectedScene].c_str()))
	{
		for (auto scene : m_SceneSelection)
		{
			bool selected = (m_SelectedScene == scene.first);
			if (ImGui::Selectable(scene.second.c_str(), selected))
			{
				m_SelectedScene = scene.first;
			}
		}
		ImGui::EndCombo();
	}
	if (m_ModelLoader.HasPending())
	{
		ImGui::Separator();
		ImGui::TextDisabled("Loading models...");
	}
}

void PBRLighting::BasicPBR()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_PBRLighting->Bind();
	m_PBRLighting->SetUniformMat4f("projection", projection);
	m_PBRLighting->SetUniformMat4f("view", view);
	m_PBRLighting->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_PBRLighting->SetUniform1i("bGammaCorrect", m_UseGamma);

	for (int row = 0; row < m_Rows; row++)
	{
		m_PBRLighting->SetUniform1f("metallic", (static_cast<float>(row) / static_cast<float>(m_Rows)));
		for (int col = 0; col < m_Cols; col++)
		{
			m_PBRLighting->SetUniform1f("roughness",
				(glm::clamp(static_cast<float>(col) / static_cast<float>(m_Cols), 0.05f, 1.0f)));
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(
				((col - (m_Cols / 2)) * m_Spacing),
				((row - (m_Rows / 2)) * m_Spacing),
				0.0f));
			m_PBRLighting->SetUniformMat4f("model", model);
			m_Sphere->Draw(*m_PBRLighting, m_Context.Renderer);
		}
	}

	for (int i = 0; i < m_LightData.size(); i++)
	{
		m_PBRLighting->Bind();
		m_PBRLighting->SetUniformVec3("light[" + std::to_string(i) + "].Position", m_LightData[i].first);
		m_PBRLighting->SetUniformVec3("light[" + std::to_string(i) + "].Color", m_LightData[i].second);
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightData[i].first, m_LightData[i].second);
	}
}

void PBRLighting::TexturedPBR()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_PBRTexturedLighting->Bind();
	m_PBRTexturedLighting->SetUniformMat4f("projection", projection);
	m_PBRTexturedLighting->SetUniformMat4f("view", view);
	m_PBRTexturedLighting->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_PBRTexturedLighting->SetUniform1i("bGammaCorrect", m_UseGamma);
	m_Albedo->Bind(0);
	m_Normal->Bind(1);
	m_Metallic->Bind(2);
	m_Roughness->Bind(3);
	m_AO->Bind(4);
	for (int row = 0; row < m_Rows; row++)
	{
		for (int col = 0; col < m_Cols; col++)
		{
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(
				((col - (m_Cols / 2)) * m_Spacing),
				((row - (m_Rows / 2)) * m_Spacing),
				0.0f));
			m_PBRTexturedLighting->SetUniformMat4f("model", model);
			m_Sphere->Draw(*m_PBRTexturedLighting, m_Context.Renderer);
		}
	}

	for (int i = 0; i < m_LightData.size(); i++)
	{
		m_PBRTexturedLighting->Bind();
		m_PBRTexturedLighting->SetUniformVec3("light[" + std::to_string(i) + "].Position", m_LightData[i].first);
		m_PBRTexturedLighting->SetUniformVec3("light[" + std::to_string(i) + "].Color", m_LightData[i].second);
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightData[i].first, m_LightData[i].second);
	}
}

void PBRLighting::ModelPBR()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_PBRModelLighting->Bind();
	m_PBRModelLighting->SetUniformMat4f("projection", projection);
	m_PBRModelLighting->SetUniformMat4f("view", view);
	m_PBRModelLighting->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_PBRModelLighting->SetUniform1i("bGammaCorrect", m_UseGamma);
	model = glm::mat4(1.0);
	m_PBRModelLighting->SetUniformMat4f("model", model);
	m_Backpack->Draw(*m_PBRModelLighting, m_Context.Renderer);

	for (int i = 0; i < m_LightData.size(); i++)
	{
		m_PBRModelLighting->Bind();
		m_PBRModelLighting->SetUniformVec3("light[" + std::to_string(i) + "].Position", m_LightData[i].first);
		m_PBRModelLighting->SetUniformVec3("light[" + std::to_string(i) + "].Color", m_LightData[i].second);
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightData[i].first, m_LightData[i].second);
	}
}

