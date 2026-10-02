#include "DepthTesting.h"

#include "Scene/SceneContext.h"
#include "Graphics/Renderer.h"
#include "Graphics/ColorPalette.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"

#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Grid.h"

#include "imgui/imgui.h"

DepthTesting::DepthTesting(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	m_Cube = std::make_unique<Cube>();
	m_Plane = std::make_unique<Plane>();
	m_Grid = std::make_unique<Grid>(50);

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/DepthTest/DepthTest.glsl");
	m_GridShader = std::make_unique<Shader>("res/shaders/GridLine/Line.glsl");
	m_CubeTexture = std::make_unique<Texture>("res/textures/marble.jpg");
	m_PlaneTexture = std::make_unique<Texture>("res/textures/metal.png");

	m_CubeTexture->LoadPixels();
	m_PlaneTexture->LoadPixels();

	m_CubeTexture->Upload();
	m_PlaneTexture->Upload();

	m_Shader->SetUniform1i("texture1", 0);

	m_CubePositions.push_back(glm::vec3( 2.0f, 0.5f, 0.0f ));
	m_CubePositions.push_back(glm::vec3(-2.0f, 0.5f, 0.0f));

}

DepthTesting::~DepthTesting()
{}

void DepthTesting::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 1000.0f);

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniform1i("bDepthTest", m_DepthTest);
	m_Shader->SetUniform1f("near", m_ZNear);
	m_Shader->SetUniform1f("far", m_ZFar);
	m_Shader->SetUniform1f("UV", 1.0f);

	m_GridShader->Bind();
	m_GridShader->SetUniformMat4f("projection", projection);
	m_GridShader->SetUniformMat4f("view", view);
	

	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_CubePositions[i]);
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("model", model);
		m_CubeTexture->Bind(0);
		m_Cube->Draw(*m_Shader, m_Context.Renderer);
	}
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("model", model);
	m_Shader->SetUniform1f("UV", 5.0f);
	m_PlaneTexture->Bind(0);
	m_Plane->Draw(*m_Shader, m_Context.Renderer);

	model = glm::mat4(1.0f);
	m_GridShader->Bind();
	m_GridShader->SetUniformMat4f("view", view);
	m_GridShader->SetUniformMat4f("projection", projection);
	m_GridShader->SetUniformMat4f("model", model);
	m_GridShader->SetUniformVec3("cameraPos", m_Camera.GetPosition());
	m_GridShader->SetUniform1f("maxDistance", 50.0f);
	m_GridShader->SetUniform1f("minDistance", 1.0f);
	m_Grid->Draw(*m_GridShader, m_Context.Renderer);
}

void DepthTesting::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Checkbox("Depth Test", &m_DepthTest);
		if (m_DepthTest)
		{
			ImGui::Separator();
			ImGui::Text("CTRL+LMB to manually enter value");
			ImGui::SliderFloat("Near Z-Depth", &m_ZNear, 0.01f, 1.0f);
			ImGui::SliderFloat("Far Z-Depth", &m_ZFar, 1.0f, 50.0f);
		}
	}
	ImGui::End();
}
