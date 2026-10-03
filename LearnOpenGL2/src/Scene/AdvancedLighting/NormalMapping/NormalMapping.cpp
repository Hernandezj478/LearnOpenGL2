#include "NormalMapping.h"
#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Scene/SceneContext.h"


NormalMapping::NormalMapping(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	m_WallAlbedo = std::make_unique<Texture>("res/textures/brickwall.jpg");
	m_WallNormal = std::make_unique<Texture>("res/textures/brickwall_normal.jpg");
	
	m_WallNormal->LoadPixels();
	m_WallNormal->InvertGChannel();
	m_WallNormal->Upload();

	m_WallAlbedo->SyncTexture();

	m_Wall = std::make_unique<Plane>();
	m_Light = std::make_unique<LightMarker>();

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/NormalMapping/NormalMapping.glsl");

	m_Shader->Bind();
	m_Shader->SetUniform1i("wallAlbedo", 0);
	m_Shader->SetUniform1i("wallNormal", 1);

	m_LightPosition = glm::vec3(0.5f, 1.0f, 0.3f);


}

void NormalMapping::Render()
{
	m_Context.Renderer.Clear(DARK_GREY, COLOR_DEPTH);

	// Render Here
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 100.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0);

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformVec3("lightPos", m_LightPosition);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_WallAlbedo->Bind(0);
	m_WallNormal->Bind(1);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, (float)glfwGetTime(), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	m_Shader->SetUniformMat4f("model", model);
	m_Wall->Draw(*m_Shader, m_Context.Renderer);
	m_Shader->Unbind();

	m_Light->Draw(m_Context.Renderer, view, projection, m_LightPosition, WHITE, 0.8f);
	model = glm::mat4(1.0);
	m_Shader->SetUniformMat4f("model", model);
}

