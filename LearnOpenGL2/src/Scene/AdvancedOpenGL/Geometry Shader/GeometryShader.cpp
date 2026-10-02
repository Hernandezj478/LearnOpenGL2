#include "GeometryShader.h"
#include "Graphics/ColorPalette.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Buffers/UniformBuffer.h"

#include "Scene/SceneContext.h"
#include <imgui/imgui.h>

GeometryShader::GeometryShader(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	m_Loader.Request([]()->std::unique_ptr<Model>
		{
			return std::make_unique<Model>("res/meshes/Backpack/Backpack.gltf"); 
		});
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/GeometryShader/Default.glsl");
	m_CubePointShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/GeometryShader/PrimativePoints.glsl");
	m_NormalShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/GeometryShader/NormalVisualization.glsl");
	m_PlanePointShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/GeometryShader/PlanePoints.glsl");
	m_PointHouseShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/GeometryShader/HouseGeometryShader.glsl");
	m_ExplosionShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/GeometryShader/ExplodeModel.glsl");

	m_Cube = std::make_unique<Cube>();
	m_Plane = std::make_unique<Plane>();

	m_UBO = std::make_unique<UniformBuffer>("MVP", 0);
	m_UBO->Bind();
	m_UBO->CreateBufferObject(nullptr, 2 * sizeof(glm::mat4));
	m_UBO->BindBase();

	m_UBO->AddShader(*m_Shader);
	m_UBO->AddShader(*m_CubePointShader);
	m_UBO->AddShader(*m_NormalShader);
	m_UBO->AddShader(*m_PlanePointShader);
	m_UBO->AddShader(*m_PointHouseShader);
	m_UBO->AddShader(*m_ExplosionShader);

	glm::mat4 projection = glm::perspective(m_Camera.GetFOV(), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	m_UBO->Bind();
	m_UBO->SetData(0, sizeof(glm::mat4), glm::value_ptr(projection));

	m_OptionMap[PRIMPOINT] = "Primative Points";
	m_OptionMap[PLANEPTS] = "Plane Primitive Points";
	m_OptionMap[HOUSEPTS] = "House Points Geometry";
	m_OptionMap[EXPLODE] = "Model Explosion";
	m_OptionMap[HARYMODEL] = "Model Normal Visual";
}

GeometryShader::~GeometryShader() = default;

void GeometryShader::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 view = m_Camera.GetViewMatrix();
	m_UBO->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glm::mat4 model = glm::mat4(1.0f);

	switch (m_SceneOption)
	{
	case PRIMPOINT:
		m_CubePointShader->Bind();
		model = glm::mat4(1.0f);
		m_CubePointShader->SetUniformMat4f("model", model);
		m_Cube->DrawPoints(*m_CubePointShader, m_Context.Renderer);
		break;
	case PLANEPTS:
		m_PlanePointShader->Bind();
		model = glm::translate(model, glm::vec3(0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0, 0.0f));
		m_PlanePointShader->SetUniformMat4f("model", model);
		m_Plane->DrawPoints(*m_PlanePointShader, m_Context.Renderer);
		break;
	case HOUSEPTS:
		m_PointHouseShader->Bind();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0, 0.0f));
		m_PointHouseShader->SetUniformMat4f("model", model);
		m_Plane->DrawPoints(*m_PointHouseShader, m_Context.Renderer);
		break;
	case EXPLODE:
		m_ExplosionShader->Bind();
		model = glm::mat4(1.0f);
		m_ExplosionShader->SetUniformMat4f("model", model);
		m_ExplosionShader->SetUniform1f("time", glfwGetTime());
		for (int i = 0; i < m_Models.size(); i++)
		{
			m_Models[i]->Draw(*m_ExplosionShader, m_Context.Renderer);
		}
		break;
	case HARYMODEL:
		m_Shader->Bind();
		model = glm::mat4(1.0f);
		m_Shader->SetUniformMat4f("model", model);

		for (int i = 0; i < m_Models.size(); i++)
		{
			m_Models[i]->Draw(*m_Shader, m_Context.Renderer);
		}

		m_NormalShader->Bind();
		m_NormalShader->SetUniformMat4f("model", model);

		for (int i = 0; i < m_Models.size(); i++)
		{
			m_Models[i]->Draw(*m_NormalShader, m_Context.Renderer);
		}
		break;
	}
}

void GeometryShader::Update(float deltaTime)
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
		}
	);
}

void GeometryShader::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("DEBUG", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		if (ImGui::BeginCombo("Shader selection", m_OptionMap[m_SceneOption].c_str()))
		{
			for (auto option : m_OptionMap)
			{
				bool selected = (option.second == m_OptionMap[m_SceneOption]);
				if (ImGui::Selectable(option.second.c_str(), selected))
				{
					m_SceneOption = option.first;
				}
			}
			ImGui::EndCombo();
		}
		if (m_Loader.HasPending())
		{
			ImGui::Separator();
			ImGui::Text("Loading Models...");
		}
	}
	ImGui::End();
}
