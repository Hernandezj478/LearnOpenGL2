#include "LightingMaps.h"

#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/ColorPalette.h"
#include "Graphics/Shapes/Cube.h"

#include "Scene/SceneContext.h"

LightingMaps::LightingMaps(const SceneContext& context) : Scene3D(context)
{
	m_CubeShader = std::make_unique<Shader>("res/shaders/Lighting/LightingMaps.glsl");
	m_DiffuseTexture = std::make_unique<Texture>("res/textures/container2.png");
	m_SpecularTexture = std::make_unique<Texture>("res/textures/container2_specular.png");
	m_Cube = std::make_unique<Cube>();

	m_LightPos = glm::vec3(1.2, 1.5f, 2.0f);

	m_DiffuseTexture->SetFlipImage(false);
	m_SpecularTexture->SetFlipImage(false);

	m_DiffuseTexture->LoadPixels();
	m_SpecularTexture->LoadPixels();

	m_DiffuseTexture->Upload();
	m_SpecularTexture->Upload();

	glEnable(GL_DEPTH_TEST);
	m_CubeShader->Bind();
	m_CubeShader->SetUniform1i("material.diffuse", 0);
	m_CubeShader->SetUniform1i("material.specular", 1);
	m_CubeShader->SetUniform1f("material.shininess", 64.0f);
	m_CubeShader->Unbind();
}

LightingMaps::~LightingMaps() = default;

void LightingMaps::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(m_Camera.GetFOV(), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_CubeShader->Bind();
	m_CubeShader->SetUniformMat4f("projection", projection);
	m_CubeShader->SetUniformMat4f("view", view);
	m_CubeShader->SetUniformMat4f("model", model);
	m_CubeShader->SetUniformVec3("viewPos", m_Camera.GetPosition());

	m_CubeShader->SetUniformVec3("light.position", m_LightPos);
	m_CubeShader->SetUniformVec3("light.ambient", GREY);
	m_CubeShader->SetUniformVec3("light.diffuse", GREY);
	m_CubeShader->SetUniformVec3("light.specular", WHITE);
	m_DiffuseTexture->Bind(0);
	m_SpecularTexture->Bind(1);

	m_Cube->Draw(*m_CubeShader, m_Context.Renderer);

	m_LightMarker.Draw(m_Context.Renderer, view, projection, m_LightPos, GREY);
}
