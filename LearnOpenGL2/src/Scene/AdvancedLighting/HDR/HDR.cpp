#include "HDR.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/Framebuffer.h"
#include "Graphics/Buffers/Renderbuffer.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>
#include <string>

HDR::HDR(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);

	m_ScreenQuad = std::make_unique<Plane2D>();
	m_Cube = std::make_unique<Cube>();
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/HDR/Lighting.glsl");
	m_hdrShader = std::make_unique<Shader>("res/shaders/AdvancedLighting/HDR/HDR.glsl");

	m_WoodTexture = std::make_unique<Texture>("res/textures/wood.png");
	m_WoodTexture->SetHDR(true);
	m_WoodTexture->SetFlipImage(false);
	m_WoodTexture->SetWrapType(ECLAMP);
	m_WoodTexture->SetGammaCorrection(true);
	m_WoodTexture->LoadPixels();
	m_WoodTexture->Upload();

	m_hdrFBO = std::make_unique<FrameBuffer>();
	m_Colorbuffer = std::make_unique<RenderTexture>();
	m_Colorbuffer->CreateColorbufferHDR(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight(), GL_RGBA16F);

	m_rboDepth = std::make_unique<RenderBuffer>();
	m_rboDepth->Bind();
	m_rboDepth->CreateStorage(GL_DEPTH_COMPONENT, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());

	m_hdrFBO->Bind();
	m_hdrFBO->AttachColorBuffer(*m_Colorbuffer);
	m_hdrFBO->AttachRenderBuffer(*m_rboDepth, GL_DEPTH_ATTACHMENT);
	m_hdrFBO->FrameBufferComplete();

	m_LightPositions =
	{
		glm::vec3( 0.0f,  0.0f, -49.5f),
		glm::vec3(-1.4f, -1.9f, -9.0f),
		glm::vec3( 0.0f, -1.8f, -4.0f),
		glm::vec3( 0.8f, -1.7f, -6.0f)
	};

	m_LightColors =
	{
		glm::vec3(200.0f, 200.0f, 200.0f),
		glm::vec3(0.1f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.2f),
		glm::vec3(0.0f, 0.1f, 0.0f)
	};

	m_Shader->Bind();
	m_Shader->SetUniform1i("diffuseTexture", 0);
	m_hdrShader->Bind();
	m_hdrShader->SetUniform1i("hdrBuffer", 0);

	m_Light = std::make_unique<LightMarker>();
}

void HDR::Render()
{
	m_Context.Renderer.Clear(DARK_GREY, COLOR_DEPTH);

	m_hdrFBO->Bind();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);

	for (int i = 0; i < m_LightPositions.size(); i++)
	{
		m_Shader->Bind();
		m_Shader->SetUniformVec3("lights[" + std::to_string(i) + "].Position", m_LightPositions[i]);
		m_Shader->SetUniformVec3("lights[" + std::to_string(i) + "].Color", m_LightColors[i]);
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightPositions[i], m_LightColors[i]);
	}
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniform1i("size", m_LightPositions.size());
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0, 0.0, -25.0));
	model = glm::scale(model, glm::vec3(5.5f, 5.5f, 60.5f));
	m_Shader->SetUniformMat4f("model", model);
	m_WoodTexture->Bind();
	m_Cube->Draw(*m_Shader, m_Context.Renderer);
	m_hdrFBO->Unbind();

	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
	m_hdrShader->Bind();
	m_hdrShader->SetUniform1f("exposure", m_Exposure);
	m_Colorbuffer->Bind();
	m_ScreenQuad->Draw(*m_hdrShader, m_Context.Renderer);
}

void HDR::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("HDR Testing", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::SliderFloat("Exposure", &m_Exposure, 0.1f, 10.0f);
	}
	ImGui::End();
}
