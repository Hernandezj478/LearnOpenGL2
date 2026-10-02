#include "ModelLoading.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Graphics/Model/Model.h"

#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

ModelLoading::ModelLoading(const SceneContext& context) : Scene3D(context)
{
	m_Shader = std::make_unique<Shader>("res/shaders/ModelLoading/ModelLoading.glsl");
	std::vector<std::string> paths = { 
		"res/meshes/Backpack/Backpack.gltf", 
		"res/meshes/Backpack_Military/Backpack_Military.gltf",
		"res/meshes/Container/container.fbx"};
	for (std::string path : paths)
	{
		m_Loader.Request([path]() -> std::unique_ptr<Model>
			{
				return std::make_unique<Model>(path);
			});
	}
	//m_Backpack = std::make_unique<Model>("res/meshes/Backpack/Backpack.gltf");

	glEnable(GL_DEPTH_TEST);
	m_LightPosition=(glm::vec3(0.0f, 1.0f, 2.0f));
}

ModelLoading::~ModelLoading()
{}

void ModelLoading::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(m_Camera.GetFOV(), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());

	m_Shader->SetUniformVec3("light.position", m_LightPosition);
	m_Shader->SetUniformVec3("light.ambient", glm::vec3(0.2f));
	m_Shader->SetUniformVec3("light.diffuse", glm::vec3(0.8f));
	m_Shader->SetUniformVec3("light.specular", glm::vec3(1.0f));
	m_Shader->SetUniform1f("light.constant", 1.0f);
	m_Shader->SetUniform1f("light.linear", 0.09f);
	m_Shader->SetUniform1f("light.quadratic", 0.032);
	m_Shader->SetUniform1f("shininess", 32.0f);

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(i * 2.5f, 0.0f, 0.0f));
		m_Shader->SetUniformMat4f("model", model);
		m_Models[i]->Draw(*m_Shader, m_Context.Renderer);
	}
}

void ModelLoading::Update(float deltaTime)
{
	Scene3D::Update(deltaTime);

	m_Loader.Update(
		[this](std::unique_ptr<Model> model)
		{
			model->Finalize();
			m_Models.push_back(std::move(model));
		},
		[this](const std::exception& e)
		{
			m_LastLoadError = e.what();
		});
}

void ModelLoading::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (m_Loader.HasPending())
	{
		if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text("Loading models...");
		}
		ImGui::End();
	}
	if (!m_LastLoadError.empty())
	{
		if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", m_LastLoadError.c_str());
		}
		ImGui::End();
	}
}
