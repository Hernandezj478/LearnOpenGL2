#include "Bloom.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffers/RenderBuffer.h"
#include "Graphics/Buffers/FrameBuffer.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Scene/SceneContext.h"


#include <imgui/imgui.h>

Bloom::Bloom(const SceneContext& context) : Scene3D(context)
{
	unsigned int ColorbufferCount = 2;

	glEnable(GL_DEPTH_TEST);
	m_Ground = std::make_unique<Plane>();
	m_ScreenQuad = std::make_unique<Plane2D>();
	m_Cube = std::make_unique<Cube>();

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/Bloom/Lighting.glsl");
	m_Blur = std::make_unique<Shader>("res/shaders/AdvancedLighting/Bloom/Blur.glsl");
	m_Bloom = std::make_unique<Shader>("res/shaders/AdvancedLighting/Bloom/Bloom.glsl");

	m_WoodTexture = std::make_unique<Texture>("res/textures/wood.png");
	m_ContainerTexture = std::make_unique<Texture>("res/textures/container2.png");

	m_WoodTexture->SetHDR(true);
	m_WoodTexture->SetFlipImage(false);
	m_WoodTexture->SetWrapType(REPEAT);
	m_WoodTexture->SetGammaCorrection(true);
	m_WoodTexture->SyncTexture();

	m_ContainerTexture->SetHDR(true);
	m_ContainerTexture->SetFlipImage(false);
	m_ContainerTexture->SetWrapType(REPEAT);
	m_ContainerTexture->SetGammaCorrection(true);
	m_ContainerTexture->SyncTexture();

	m_hdrFBO = std::make_unique<FrameBuffer>();
	m_Colorbuffers = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight(), ColorbufferCount);
	m_Colorbuffers->SetMultiAttachment(true);
	m_hdrFBO->Bind();
	m_Colorbuffers->CreateTextureBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_hdrFBO->AttachColorBuffer(*m_Colorbuffers);
	m_hdrFBO->ConfigureColorAttachments(2);

	m_rboDepth = std::make_unique<RenderBuffer>();
	m_rboDepth->Bind();
	m_rboDepth->CreateStorage(GL_DEPTH_COMPONENT, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_hdrFBO->AttachRenderBuffer(*m_rboDepth, GL_DEPTH_ATTACHMENT);
	m_hdrFBO->FrameBufferComplete();
	m_hdrFBO->Unbind();

	// ping-pong framebuffer for blurring
	m_PingPongFBO = std::make_unique<FrameBuffer>(2);
	m_PingPongColorbuffers = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight(), 2);
	m_PingPongColorbuffers->SetMultiAttachment(false);
	m_PingPongColorbuffers->CreateTextureBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);
	m_PingPongFBO->Bind();
	m_PingPongFBO->AttachColorBuffer(*m_PingPongColorbuffers);
	m_PingPongFBO->FrameBufferComplete();
	m_PingPongFBO->Unbind();

	// Lighting info
	//						Position  | Color
	m_LightData =
	{
		std::pair<glm::vec3, glm::vec3>(glm::vec3(0.0f, 0.5f,  1.5f), glm::vec3(5.0f, 5.0f,  5.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-4.0f, 0.5f, -3.0f), glm::vec3(10.0f, 0.0f,  0.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(3.0f, 0.5f,  1.0f), glm::vec3(0.0f, 0.0f, 15.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-0.8f, 2.4f, -1.0f), glm::vec3(0.0f, 5.0f,  0.0f)),
	};

	// Cube positions
	//					Position | Scale
	m_CubeData =
	{
		std::pair<glm::vec3, glm::vec3>(glm::vec3(0.0f,  1.5f,  0.0f), glm::vec3(0.5f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(2.0f,  0.0f,  1.0f), glm::vec3(0.5f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-1.0f, -1.0f,  2.0f), glm::vec3(1.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(0.0f,  2.7f,  4.0f), glm::vec3(1.25f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-2.0f,  1.0f, -3.0f), glm::vec3(1.0f)),
		std::pair<glm::vec3, glm::vec3>(glm::vec3(-3.0f,  0.0f,  0.0f), glm::vec3(0.5f))
	};

	m_Shader->Bind();
	m_Shader->SetUniform1i("diffuseTexture", 0);
	m_Blur->Bind();
	m_Blur->SetUniform1i("image", 0);
	m_Bloom->Bind();
	m_Bloom->SetUniform1i("lighting", 0);
	m_Bloom->SetUniform1i("blur", 1);

	m_Light = std::make_unique<LightMarker>();
}

void Bloom::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	m_hdrFBO->Bind();
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_Shader->SetUniform1i("numLights", m_LightData.size());

	for (int i = 0; i < m_LightData.size(); i++)
	{
		m_Shader->Bind();
		m_Shader->SetUniformVec3("lights[" + std::to_string(i) + "].Position", m_LightData[i].first);
		m_Shader->SetUniformVec3("lights[" + std::to_string(i) + "].Color", m_LightData[i].second);
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightData[i].first, m_LightData[i].second);
	}
	m_Shader->Bind();
	for (int i = 0; i < m_CubeData.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_CubeData[i].first);
		model = glm::scale(model, m_CubeData[i].second);
		m_Shader->SetUniformMat4f("model", model);
		m_ContainerTexture->Bind();
		m_Cube->Draw(*m_Shader, m_Context.Renderer);
	}
	model = glm::mat4(1.0);
	model = glm::scale(model, glm::vec3(10.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("model", model);
	m_WoodTexture->Bind();
	m_Ground->Draw(*m_Shader, m_Context.Renderer);
	m_hdrFBO->Unbind();

	bool horizontal = true, first_iteration = true;
	int amount = 10;
	m_Blur->Bind();
	for (unsigned int i = 0; i < amount; i++)
	{
		unsigned int fboIndex = horizontal ? 0 : 1;
		m_PingPongFBO->Bind(fboIndex);
		m_Blur->SetUniform1i("horizontal", horizontal);
		if (first_iteration)
		{
			m_Colorbuffers->Bind(0, 1);
		}
		else
		{
			int sampleIndex = horizontal ? 1 : 0;
			m_PingPongColorbuffers->Bind(0, sampleIndex);
		}
		m_ScreenQuad->Draw(*m_Blur, m_Context.Renderer);
		horizontal = !horizontal;
		if (first_iteration)
		{
			first_iteration = false;
		}
	}
	m_PingPongFBO->Unbind();

	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
	m_Bloom->Bind();
	m_Colorbuffers->Bind(0, 0);
	m_PingPongColorbuffers->Bind(1, !horizontal);
	m_Bloom->SetUniform1i("bloom", m_UseBloom);
	m_Bloom->SetUniform1f("exposure", m_Exposure);
	m_ScreenQuad->Draw(*m_Bloom, m_Context.Renderer);
}

void Bloom::OnGui()
{
	ImGui::SetNextWindowSize(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Bloom", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Checkbox("Bloom", &m_UseBloom);
		ImGui::SliderFloat("Exposure", &m_Exposure, 0.1f, 10.0f);
	}
	ImGui::End();
}
