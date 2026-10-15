#include "SSAO.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/FrameBuffer.h"
#include "Graphics/Buffers/RenderBuffer.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Sphere.h"
#include "Scene/SceneContext.h"

#include <random>
#include <imgui/imgui.h>

SSAO::SSAO(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	m_LightPosition = glm::vec3(2.0f, 4.0f, -2.0f);
	m_LightColor = LIGHT_GREY;
	m_RoomTexture = std::make_unique<Texture>("res/textures/GreyboxTexture.png");
	m_RoomTexture->SyncTexture();
	m_Light = std::make_unique<LightMarker>();

	m_GeometryPass = std::make_unique<Shader>("res/shaders/AdvancedLighting/SSAO/GeometryPass.glsl");
	m_ssaoPass = std::make_unique<Shader>("res/shaders/AdvancedLighting/SSAO/SSAO.glsl");
	m_BlurPass = std::make_unique<Shader>("res/shaders/AdvancedLighting/SSAO/SSAO_Blur.glsl");
	m_LightingPass = std::make_unique<Shader>("res/shaders/AdvancedLighting/SSAO/LightingPass.glsl");

	m_Backpack = std::make_unique<Model>("res/meshes/Backpack/Backpack.gltf");
	m_Backpack->Finalize();
	m_Room = std::make_unique<Cube>();
	m_ScreenQuad = std::make_unique<Plane2D>();

	m_GBuffer = std::make_unique<FrameBuffer>();
	m_Position = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_Normal = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_AlbedoSpec = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	
	m_Position->SetMinFilter(GL_NEAREST);
	m_Position->SetMagFilter(GL_NEAREST);
	m_Position->SetWrapS(GL_CLAMP_TO_EDGE);
	m_Position->SetWrapT(GL_CLAMP_TO_EDGE);
	m_Position->CreateTextureBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);
	
	m_Normal->SetMinFilter(GL_NEAREST);
	m_Normal->SetMagFilter(GL_NEAREST);
	m_Normal->CreateTextureBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);
	
	m_AlbedoSpec->SetMinFilter(GL_NEAREST);
	m_AlbedoSpec->SetMagFilter(GL_NEAREST);
	m_AlbedoSpec->CreateTextureBuffer(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	m_rboDepth = std::make_unique<RenderBuffer>();
	m_rboDepth->Bind();
	m_rboDepth->CreateStorage(GL_DEPTH_COMPONENT, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	
	m_GBuffer->Bind();
	m_GBuffer->AttachColorBuffer(*m_Position, 0);
	m_GBuffer->AttachColorBuffer(*m_Normal, 1);
	m_GBuffer->AttachColorBuffer(*m_AlbedoSpec, 2);
	m_GBuffer->AttachRenderBuffer(*m_rboDepth, GL_DEPTH_ATTACHMENT);
	m_GBuffer->ConfigureColorAttachments(3);
	m_GBuffer->FrameBufferComplete();
	m_GBuffer->Unbind();

	m_ssaoFBO = std::make_unique<FrameBuffer>();
	m_BlurFBO = std::make_unique<FrameBuffer>();
	m_ssaoColorBuffer = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_BlurBuffer = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());

	m_ssaoColorBuffer->SetMinFilter(GL_NEAREST);
	m_ssaoColorBuffer->SetMagFilter(GL_NEAREST);
	m_ssaoColorBuffer->CreateTextureBuffer(GL_RED, GL_RED, GL_FLOAT);
	
	m_BlurBuffer->SetMinFilter(GL_NEAREST);
	m_BlurBuffer->SetMagFilter(GL_NEAREST);
	m_BlurBuffer->CreateTextureBuffer(GL_RED, GL_RED, GL_FLOAT);

	m_ssaoFBO->Bind();
	m_ssaoFBO->AttachColorBuffer(*m_ssaoColorBuffer);
	m_ssaoFBO->FrameBufferComplete();

	m_BlurFBO->Bind();
	m_BlurFBO->AttachColorBuffer(*m_BlurBuffer);
	m_BlurFBO->FrameBufferComplete();

	std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	for (unsigned int i = 0; i < 64; i++)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = static_cast<float>(i) / 64.0f;

		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		m_ssaoKernel.push_back(sample);
	}
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			0.0f);
		m_ssaoNoise.push_back(noise);
	}
	
	m_NoiseTexture = std::make_unique<RenderTexture>(4, 4);
	m_NoiseTexture->SetMinFilter(GL_NEAREST);
	m_NoiseTexture->SetMagFilter(GL_NEAREST);
	m_NoiseTexture->SetWrapS(GL_REPEAT);
	m_NoiseTexture->SetWrapT(GL_REPEAT);
	m_NoiseTexture->CreateTextureBuffer(GL_RGBA16F, GL_RGB, GL_FLOAT, m_ssaoNoise.data());

	m_ssaoPass->Bind();
	m_ssaoPass->SetUniform3fv("samples", m_ssaoKernel.data(), m_ssaoKernel.size());
	m_ssaoPass->SetUniform1i("kernelSize", m_ssaoKernel.size());
	m_ssaoPass->SetUniform1f("radius", 0.5f);
	m_ssaoPass->SetUniform1f("bias", 0.025f);
	glm::vec2 screenSize = m_Camera.GetScreenDimension();
	glm::vec2 noiseScale = screenSize / 4.0f;
	m_ssaoPass->SetUniformVec2("noiseScale", noiseScale);

	m_ssaoPass->SetUniform1i("gPosition", 0);
	m_ssaoPass->SetUniform1i("gNormal", 1);
	m_ssaoPass->SetUniform1i("texNoise", 2);

	m_BlurPass->Bind();
	m_BlurPass->SetUniform1i("ssaoInput", 0);
	
	m_LightingPass->Bind();
	m_LightingPass->SetUniform1i("gPosition", 0);
	m_LightingPass->SetUniform1i("gNormal", 1);
	m_LightingPass->SetUniform1i("gAlbedoSpec", 2);
	m_LightingPass->SetUniform1i("ssao", 3);

}

void SSAO::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	m_GBuffer->Bind();
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);

	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.f);

	m_GeometryPass->Bind();
	m_GeometryPass->SetUniformMat4f("projection", projection);
	m_GeometryPass->SetUniformMat4f("view", view);
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0, 4.75, 0.0));
	model = glm::scale(model, glm::vec3(10.f));
	m_GeometryPass->SetUniformMat4f("model", model);
	m_GeometryPass->SetUniform1i("invertNormals", true);
	m_RoomTexture->Bind();
	m_Room->Draw(*m_GeometryPass, m_Context.Renderer);

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	m_GeometryPass->SetUniformMat4f("model", model);
	m_GeometryPass->SetUniform1i("invertNormals", false);
	m_Backpack->Draw(*m_GeometryPass, m_Context.Renderer);

	m_GBuffer->Unbind();

	if (m_UseSSAO)
	{
		m_ssaoFBO->Bind();
		m_Context.Renderer.ClearBufferBits(COLOR);
		m_ssaoPass->Bind();
		m_ssaoPass->SetUniformMat4f("projection", projection);
		m_Position->Bind(0);
		m_Normal->Bind(1);
		m_NoiseTexture->Bind(2);
		m_ScreenQuad->Draw(*m_ssaoPass, m_Context.Renderer);
		m_ssaoFBO->Unbind();
	
		m_BlurFBO->Bind();
		m_Context.Renderer.ClearBufferBits(COLOR);
		m_BlurPass->Bind();
		m_ssaoColorBuffer->Bind(0);
		m_ScreenQuad->Draw(*m_BlurPass, m_Context.Renderer);
		m_BlurFBO->Unbind();
	}

	const float linear = 0.09f;
	const float quadratic = 0.032f;
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
	m_LightingPass->Bind();
	m_LightingPass->SetUniformVec3("light.Position", view * glm::vec4(m_LightPosition, 1.0));
	m_LightingPass->SetUniformVec3("light.Color", m_LightColor);
	m_LightingPass->SetUniform1f("light.Linear", linear);
	m_LightingPass->SetUniform1f("light.Quadratic", quadratic);
	m_LightingPass->SetUniform1i("bUseSSAO", m_UseSSAO);
	m_Position->Bind(0);
	m_Normal->Bind(1);
	m_AlbedoSpec->Bind(2);
	if(m_UseSSAO)
		m_ssaoColorBuffer->Bind(3);
	m_ScreenQuad->Draw(*m_LightingPass, m_Context.Renderer);

	m_GBuffer->BlitBuffer(nullptr, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight(), GL_DEPTH_BUFFER_BIT);
	m_GBuffer->Unbind();
	m_Light->Draw(m_Context.Renderer, view, projection, m_LightPosition, m_LightColor);
}

void SSAO::OnGui()
{
	ImGui::Checkbox("SSAO", &m_UseSSAO);
}

float SSAO::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}
