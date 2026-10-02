#include "Instancing.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"

#include "Graphics/Model/Model.h"
#include "Graphics/Shapes/Sphere.h"
#include "Graphics/Shapes/Plane.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

Instancing::Instancing(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);

	std::vector<std::string> filePaths =
	{
		"res/textures/NightSky.jpg"
	};
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Instancing/Instancing.glsl");
	m_SkyboxShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Instancing/Skybox.glsl");
	m_PlaneShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Instancing/ColorPlane.glsl");

	m_SkyboxTexture = std::make_unique<Texture>("res/textures/NightSky_4K.jpg");
	m_Skybox = std::make_unique<Sphere>();
	m_Plane = std::make_unique < Plane>();

	m_SkyboxTexture->SyncTexture();

	unsigned int amount = 100000;
	std::vector<glm::mat4> modelMatrices;
	srand(glfwGetTime());
	float radius = 250.0f;
	float offset = 25.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		float rotAngle = float(rand() % 360);
		model = glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.4f, 0.6f, 0.8f));

		modelMatrices.push_back(model);
	}

	std::vector<glm::mat4> planeTranslations;
	offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::mat4 translation = glm::mat4(1.0);
			float a = (float)x / 10.0f + offset;
			float b = (float)y / 10.0f + offset;
			translation = glm::translate(translation, glm::vec3(a, b, 0.0f));
			translation = glm::rotate(translation, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
			translation = glm::scale(translation, glm::vec3(0.06f));
			planeTranslations.push_back(translation);
		}
	}

	m_Plane->CreateInstance(planeTranslations);

	m_Loader.Request([matrices = std::move(modelMatrices)]() mutable -> LoadedModel
		{
			LoadedModel result;
			result.model = std::make_unique<Model>("res/meshes/Asteroid/rock.obj");
			result.instanceMatrices = std::move(matrices);
			return result;
		});
	m_Loader.Request([]() -> LoadedModel
		{
			LoadedModel result;
			result.model = std::make_unique<Model>("res/meshes/Planet/planet.obj");
			return result;
		});
	m_SkyboxShader->Bind();
	m_SkyboxShader->SetUniform1i("skybox", 0);

	m_SceneMap[SIMPLE] = "Instance color planes";
	m_SceneMap[SPACE] = "Asteroid belt";

}

Instancing::~Instancing() = default;

void Instancing::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 1000.0f);
	switch (m_CurrentSelection)
	{
	case SIMPLE:
		m_PlaneShader->Bind();
		m_PlaneShader->SetUniformMat4f("view", view);
		m_PlaneShader->SetUniformMat4f("projection", projection);
		m_Plane->Draw(*m_PlaneShader, m_Context.Renderer);
		break;
	case SPACE:
		m_Shader->Bind();
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime() * 0.02f, glm::vec3(0.0f, 0.0f, 1.0f));
		m_Shader->SetUniformMat4f("model", model);
		m_Shader->SetUniformMat4f("view", view);
		m_Shader->SetUniformMat4f("projection", projection);

		for (int i = 0; i < m_Models.size(); i++)
		{
			m_Models[i]->Draw(*m_Shader, m_Context.Renderer);
		}

		glDepthFunc(GL_LEQUAL);
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(10000.0));
		m_SkyboxShader->Bind();
		m_SkyboxShader->SetUniformMat4f("projection", projection);
		m_SkyboxShader->SetUniformMat4f("view", view);
		m_SkyboxShader->SetUniformMat4f("model", model);
		m_SkyboxTexture->Bind();
		m_Skybox->Draw(*m_SkyboxShader, m_Context.Renderer);
		glDepthFunc(GL_LESS);
		break;
	}
	

	
}

void Instancing::Update(float deltaTime)
{
	Scene3D::Update(deltaTime);

	m_Loader.Update(
		[this](LoadedModel loadedModel)
		{
			loadedModel.model->Finalize();
			if (!loadedModel.instanceMatrices.empty())
			{
				loadedModel.model->CreateInstance(loadedModel.instanceMatrices);
			}
			m_Models.push_back(std::move(loadedModel.model));
		},
		[this](const std::exception& e)
		{
			m_LastLoadError = e.what();
		}
	);
}

void Instancing::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Instancing", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		if (ImGui::BeginCombo("Instance scene", m_SceneMap[m_CurrentSelection].c_str()))
		{
			for (auto scene : m_SceneMap)
			{
				bool selected = (scene.first == m_CurrentSelection);
				if (ImGui::Selectable(scene.second.c_str(), selected))
				{
					m_CurrentSelection = scene.first;
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::End();
}
