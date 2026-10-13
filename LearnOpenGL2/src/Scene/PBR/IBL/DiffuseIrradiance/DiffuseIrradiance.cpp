#include "DiffuseIrradiance.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Cubemap.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/Framebuffer.h"
#include "Graphics/Buffers/RenderBuffer.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Sphere.h"
#include "Graphics/Shapes/Skybox.h"
#include "Scene/SceneContext.h"

DiffuseIrradiance::DiffuseIrradiance(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	m_Cube = std::make_unique<Cube>();
	m_Sphere = std::make_unique<Sphere>();
	m_Light = std::make_unique<LightMarker>();
	m_Skybox = std::make_unique<Skybox>();

	m_LoftRoom = std::make_unique<Texture>("res/textures/HDR/newport_loft.hdr");
	m_LoftRoom->SetHDR(true);
	m_LoftRoom->SyncTexture();
	
	m_PBRDiffuse = std::make_unique<Shader>("res/shaders/PBR/IBL/DiffuseIrradiance/PBRDiffuse.glsl");
	m_PBRDiffuse->Bind();
	m_PBRDiffuse->SetUniformVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
	m_PBRDiffuse->SetUniform1f("ao", 1.0f);
	m_PBRDiffuse->SetUniform1f("irradianceMap", 0);

	m_EquiRectMap = std::make_unique<Shader>("res/shaders/PBR/IBL/DiffuseIrradiance/EquiRectMap.glsl");
	m_EquiRectMap->Bind();
	m_EquiRectMap->SetUniform1i("equirectMap", 0);

	m_EnvShader = std::make_unique<Shader>("res/shaders/PBR/IBL/DiffuseIrradiance/SimpleSkybox.glsl");
	m_EnvShader->Bind();
	m_EnvShader->SetUniform1i("envMap", 0);

	m_IrradianceMap = std::make_unique<Shader>("res/shaders/PBR/IBL/DiffuseIrradiance/CubemapConvolution.glsl");
	m_IrradianceMap->Bind();
	m_IrradianceMap->SetUniform1i("envMap", 0);

	m_captureFBO = std::make_unique<FrameBuffer>();
	m_captureRBO = std::make_unique<RenderBuffer>();

	m_envCubemap = std::make_unique<Cubemap>();
	m_envCubemap->SetMagFilter(GL_LINEAR);
	m_envCubemap->SetMinFilter(GL_LINEAR);
	m_envCubemap->CreateHDRCubemap(512, 512);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	m_captureFBO->Bind();
	m_captureRBO->Bind();
	m_captureRBO->CreateStorage(GL_DEPTH_COMPONENT24, 512, 512);
	m_captureFBO->AttachRenderBuffer(*m_captureRBO, GL_DEPTH_ATTACHMENT);

	m_EquiRectMap->Bind();
	m_EquiRectMap->SetUniformMat4f("projection", captureProjection);
	m_LoftRoom->Bind();
	glViewport(0, 0, 512, 512);
	m_captureFBO->Bind();
	for (unsigned int i = 0; i < 6; i++)
	{
		m_EquiRectMap->SetUniformMat4f("view", captureViews[i]);
		m_captureFBO->AttachCubemap(*m_envCubemap, i);
		m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
		m_Skybox->Draw(*m_EquiRectMap, m_Context.Renderer);
	}
	m_captureFBO->Unbind();

	m_IrradianceCubemap = std::make_unique<Cubemap>();
	m_IrradianceCubemap->CreateHDRCubemap(32, 32);

	m_captureFBO->Bind();
	m_captureRBO->Bind();
	m_captureRBO->CreateStorage(GL_DEPTH_COMPONENT24, 32, 32);

	m_IrradianceMap->Bind();
	m_IrradianceMap->SetUniformMat4f("projection", captureProjection);
	m_envCubemap->Bind();

	glViewport(0, 0, 32, 32);
	m_captureFBO->Bind();
	for (unsigned int i = 0; i < 6; i++)
	{
		m_IrradianceMap->SetUniformMat4f("view", captureViews[i]);
		m_captureFBO->AttachCubemap(*m_IrradianceCubemap, i);
		m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
		m_Skybox->Draw(*m_IrradianceMap, m_Context.Renderer);
	}
	m_captureFBO->Unbind();

	m_LightData.push_back({ glm::vec3(-10.0f,  10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3( 10.0f,  10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3( 10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
}

void DiffuseIrradiance::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_PBRDiffuse->Bind();
	m_PBRDiffuse->SetUniformMat4f("projection", projection);
	m_PBRDiffuse->SetUniformMat4f("view", view);
	m_PBRDiffuse->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_PBRDiffuse->SetUniform1i("bGammaCorrect", m_UseGamma);
	m_IrradianceCubemap->Bind();
	for (int row = 0; row < m_Rows; row++)
	{
		m_PBRDiffuse->SetUniform1f("metallic", (static_cast<float>(row) / static_cast<float>(m_Rows)));
		for (int col = 0; col < m_Cols; col++)
		{
			m_PBRDiffuse->SetUniform1f("roughness",
				(glm::clamp(static_cast<float>(col) / static_cast<float>(m_Cols), 0.05f, 1.0f)));
			model = glm::mat4(1.0);
			model = glm::translate(model, glm::vec3(
				((col - (m_Cols / 2)) * m_Spacing),
				((row - (m_Rows / 2)) * m_Spacing),
				-5.0f));
			m_PBRDiffuse->SetUniformMat4f("model", model);
			m_Sphere->Draw(*m_PBRDiffuse, m_Context.Renderer);
		}
	}
	for (int i = 0; i < m_LightData.size(); i++)
	{
		m_PBRDiffuse->Bind();
		m_PBRDiffuse->SetUniformVec3("light[" + std::to_string(i) + "].Position", m_LightData[i].first);
		m_PBRDiffuse->SetUniformVec3("light[" + std::to_string(i) + "].Color", m_LightData[i].second);
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightData[i].first, m_LightData[i].second);
	}

	m_EnvShader->Bind();
	m_EnvShader->SetUniformMat4f("projection", projection);
	m_EnvShader->SetUniformMat4f("view", view);
	m_envCubemap->Bind();
	m_Skybox->Draw(*m_EnvShader, m_Context.Renderer);

}
