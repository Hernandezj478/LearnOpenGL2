#include "StencilTesting.h"

#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Grid.h"
#include "Graphics/ColorPalette.h"

#include "Scene/SceneContext.h"

StencilTesting::StencilTesting(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);

	m_Cube = std::make_unique<Cube>();
	m_Plane = std::make_unique<Plane>();
	m_Grid = std::make_unique<Grid>(100);

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/DepthTest/DepthTest.glsl");
	m_SingleColor = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/StencilTest/SingleColor.glsl");
	m_GridShader = std::make_unique<Shader>("res/shaders/GridLine/Line.glsl");
	m_CubeTexture = std::make_unique<Texture>("res/textures/marble.jpg");
	m_PlaneTexture = std::make_unique<Texture>("res/textures/metal.png");

	m_CubeTexture->SyncTexture();
	m_PlaneTexture->SyncTexture();

	m_Shader->SetUniform1i("texture1", 0);

	m_CubePositions.push_back(glm::vec3(2.0f, 0.5f, 0.0f));
	m_CubePositions.push_back(glm::vec3(-2.0f, 0.5f, 0.0f));
}

StencilTesting::~StencilTesting() = default;

void StencilTesting::Render()
{
	m_Context.Renderer.Clear(BLACK, ALL);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 1000.0f);
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);

	m_GridShader->Bind();
	m_GridShader->SetUniformMat4f("projection", projection);
	m_GridShader->SetUniformMat4f("view", view);

	m_SingleColor->Bind();
	m_SingleColor->SetUniformMat4f("projection", projection);
	m_SingleColor->SetUniformMat4f("view", view);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_CubePositions[i]);
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("model", model);
		m_CubeTexture->Bind();
		m_Cube->Draw(*m_Shader, m_Context.Renderer);
	}

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);

	glStencilMask(0x00);
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("model", model);
	m_PlaneTexture->Bind();
	m_Plane->Draw(*m_Shader, m_Context.Renderer);

	m_GridShader->Bind();
	m_GridShader->SetUniformVec3("cameraPos", m_Camera.GetPosition());
	m_GridShader->SetUniform1f("maxDistance", 10.0f);
	m_GridShader->SetUniform1f("minDistance", 1.0f);
	m_Grid->Draw(*m_GridShader, m_Context.Renderer);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	m_SingleColor->Bind();
	float scale = 1.05f;
	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_CubePositions[i]);
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		m_SingleColor->SetUniformMat4f("model", model);
		m_Cube->Draw(*m_SingleColor, m_Context.Renderer);
	}

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
}
