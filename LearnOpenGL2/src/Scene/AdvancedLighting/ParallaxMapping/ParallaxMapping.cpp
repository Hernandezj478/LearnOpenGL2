#include "ParallaxMapping.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Shapes/Plane.h"
#include "Scene/SceneContext.h"
#include <imgui/imgui.h>

ParallaxMapping::ParallaxMapping(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	m_Plane = std::make_unique<Plane>();

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/ParallaxMapping/ParallaxMapping.glsl");

	m_WallAlbedo = std::make_unique<Texture>("res/textures/Brickwall/BrickWall2_ALbedo.jpg");
	m_WallNormal = std::make_unique<Texture>("res/textures/Brickwall/BrickWall2_Normal.jpg");
	m_WallHeight = std::make_unique<Texture>("res/textures/Brickwall/BrickWall2_Height.jpg");

	m_ToyAlbedo = std::make_unique<Texture>("res/textures/wood.png");
	m_ToyNormal = std::make_unique<Texture>("res/textures/toy_box_normal.png");
	m_ToyHeight = std::make_unique<Texture>("res/textures/toy_box_disp.png");

	m_WallAlbedo->SyncTexture();
	m_WallNormal->SyncTexture();
	m_WallHeight->SyncTexture();

	m_ToyAlbedo->SyncTexture();
	m_ToyNormal->LoadPixels();
	m_ToyNormal->InvertGChannel();
	m_ToyNormal->Upload();
	m_ToyHeight->SyncTexture();

	m_Shader->Bind();
	m_Shader->SetUniform1i("albedoMap", 0);
	m_Shader->SetUniform1i("normalMap", 1);
	m_Shader->SetUniform1i("heightMap", 2);
	

	m_LightPosition = glm::vec3(0.5f, 1.0f, 0.3f);

	m_Light = std::make_unique<LightMarker>();
}

void ParallaxMapping::Render()
{
	m_Context.Renderer.Clear(DARK_GREY, COLOR_DEPTH);

	// Render Here
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 100.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0);


	m_Shader->Bind();
	//m_Shader->SetUniform1i("normalToggle", bNormal);
	//m_Shader->SetUniform1i("parallaxToggle", bParallax);
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformVec3("lightPos", m_LightPosition);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_Shader->SetUniform1f("uvScale", 10.0f);
	m_WallAlbedo->Bind(0);
	m_WallNormal->Bind(1);
	m_WallHeight->Bind(2);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	m_Shader->SetUniformMat4f("model", model);
	m_Plane->Draw(*m_Shader, m_Context.Renderer);

	m_ToyAlbedo->Bind(0);
	m_ToyNormal->Bind(1);
	m_ToyHeight->Bind(2);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	m_Shader->SetUniform1f("uvScale", 1.0f);
	m_Shader->SetUniformMat4f("model", model);
	m_Plane->Draw(*m_Shader, m_Context.Renderer);

	m_Light->Draw(m_Context.Renderer, view, projection, m_LightPosition, WHITE);
}

