#include "Blending.h"
#include "Common/Enums.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Grid.h"

#include "Scene/SceneContext.h"

#include <map>
#include <imgui/imgui.h>

Blending::Blending(const SceneContext& context) : Scene3D(context)
{
	srand((unsigned int)time(0));

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Cube = std::make_unique<Cube>();
	m_Plane = std::make_unique<Plane>();
	m_Grid = std::make_unique<Grid>(100);

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Blending/Blending.glsl");
	m_GridShader = std::make_unique<Shader>("res/shaders/GridLine/Line.glsl");
	m_CubeTexture = std::make_unique<Texture>("res/textures/marble.jpg");
	m_PlaneTexture = std::make_unique<Texture>("res/textures/metal.png");
	m_GrassTexture = std::make_unique<Texture>("res/textures/grass_blades.png");
	m_GroundTexture = std::make_unique<Texture>("res/textures/grass.jpg");
	m_WindowTexture = std::make_unique<Texture>("res/textures/transparent_window.png");

	m_GrassTexture->SetWrapType(ECLAMP);

	m_CubeTexture->SyncTexture();
	m_PlaneTexture->SyncTexture();
	m_GrassTexture->SyncTexture();
	m_GroundTexture->SyncTexture();
	m_WindowTexture->SyncTexture();

	m_Shader->SetUniform1i("texture1", 0);

	m_CubePositions =
	{
		{ 2.0f, 0.5f, 0.0f},
		{-2.0f, 0.5f, 0.0f}
	};
}

Blending::~Blending() = default;

void Blending::Render()
{
	int size = m_GroundSize;
	if (bGrassCountUpdate)
	{
		m_GrassPositions.clear();
		for (int i = 0; i < m_GrassCount; i++)
		{
			m_GrassPositions.push_back(glm::vec3((rand() % size) * 2 - size, 1.0f, (rand() % size) * 2 - size));
		}
		bGrassCountUpdate = false;
	}
	if (bWindowCountUpdate)
	{
		m_WindowPositions.clear();
		for (int i = 0; i < m_WindowCount; i++)
		{
			m_WindowPositions.push_back(glm::vec3((rand() % size) * 2 - size, 1.0f, (rand() % size) * 2 - size));
		}
		bWindowCountUpdate = false;
	}


	std::map<float, glm::vec3> sorted;
	for (unsigned int i = 0; i < m_WindowPositions.size(); i++)
	{
		float distance = glm::length(m_Camera.GetPosition() - m_WindowPositions[i]);
		sorted[distance] = m_WindowPositions[i];
	}

	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 1000.0f);
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniform1f("UV", 1.0f);

	//Cubes
	m_GridShader->Bind();
	m_GridShader->SetUniformMat4f("projection", projection);
	m_GridShader->SetUniformMat4f("view", view);

	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_CubePositions[i]);
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("model", model);
		m_CubeTexture->Bind();
		m_Cube->Draw(*m_Shader, m_Context.Renderer);
	}

	//Ground
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(static_cast<float>(m_GroundSize)));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("model", model);
	m_Shader->SetUniform1f("UV", m_GroundSize);
	m_GroundTexture->Bind();
	m_Plane->Draw(*m_Shader, m_Context.Renderer);

	glDisable(GL_CULL_FACE);

	//Grass
	for (int i = 0; i < m_GrassPositions.size(); i++)
	{
		//Plane 1
		model = glm::mat4(1.0);
		model = glm::translate(model, m_GrassPositions[i]);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("model", model);
		m_Shader->SetUniform1f("UV", 1.0f);
		m_GrassTexture->Bind();
		m_Plane->Draw(*m_Shader, m_Context.Renderer);
		//Plane 2
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("model", model);
		m_GrassTexture->Bind();
		m_Plane->Draw(*m_Shader, m_Context.Renderer);
	}

	//Grid
	model = glm::mat4(1.0f);

	m_GridShader->Bind();
	m_GridShader->SetUniformMat4f("view", view);
	m_GridShader->SetUniformMat4f("projection", projection);
	m_GridShader->SetUniformMat4f("model", model);
	m_GridShader->SetUniformVec3("cameraPos", m_Camera.GetPosition());
	m_GridShader->SetUniform1f("maxDistance", 40.0f);
	m_GridShader->SetUniform1f("minDistance", 1.0f);
	m_Grid->Draw(*m_GridShader, m_Context.Renderer);

	//Window
	for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); it++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, it->second);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("model", model);
		m_WindowTexture->Bind();
		m_Plane->Draw(*m_Shader, m_Context.Renderer);
	}

}

void Blending::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		if (ImGui::InputInt("Grass Count", &m_GrassCount))
		{
			bGrassCountUpdate = true;
		}
		if (ImGui::InputInt("Window Count", &m_WindowCount))
		{
			bWindowCountUpdate = true;
		}
		if (ImGui::InputInt("Ground Size", &m_GroundSize))
		{
			bGrassCountUpdate = true;
			bWindowCountUpdate = true;
		}
	}
	ImGui::End();
}
