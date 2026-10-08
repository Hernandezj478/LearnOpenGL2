#include "DeferredShading.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Graphics/Buffers/FrameBuffer.h"
#include "Graphics/Buffers/RenderBuffer.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Sphere.h"
#include "Graphics/Renderer.h"
#include "Scene/SceneContext.h"

#include <stdlib.h>
#include <time.h>
#include <imgui/imgui.h>

DeferredShading::DeferredShading(const SceneContext& context) :Scene3D(context)
{
	m_Position = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_Position->SetMinFilter(GL_NEAREST);
	m_Position->SetMagFilter(GL_NEAREST);
	m_Position->CreateTextureBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);

	m_Normal = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_Normal->SetMinFilter(GL_NEAREST);
	m_Normal->SetMagFilter(GL_NEAREST);
	m_Normal->CreateTextureBuffer(GL_RGBA16F, GL_RGBA, GL_FLOAT);

	m_AlbedoSpec = std::make_unique<RenderTexture>(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_AlbedoSpec->SetMinFilter(GL_NEAREST);
	m_AlbedoSpec->SetMagFilter(GL_NEAREST);
	m_AlbedoSpec->CreateTextureBuffer(GL_RGBA, GL_RGB, GL_UNSIGNED_BYTE);

	m_rboDepth = std::make_unique<RenderBuffer>();
	m_rboDepth->Bind();
	m_rboDepth->CreateStorage(GL_DEPTH_COMPONENT, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());

	m_GBuffer = std::make_unique<FrameBuffer>();
	m_GBuffer->Bind();
	m_GBuffer->AttachColorBuffer(*m_Position, 0);
	m_GBuffer->AttachColorBuffer(*m_Normal, 1);
	m_GBuffer->AttachColorBuffer(*m_AlbedoSpec, 2);
	m_GBuffer->AttachRenderBuffer(*m_rboDepth, GL_DEPTH_ATTACHMENT);
	m_GBuffer->ConfigureColorAttachments(3);
	m_GBuffer->FrameBufferComplete();

	m_GeometryPass = std::make_unique<Shader>("res/shaders/AdvancedLighting/DeferredShading/PositionColor.glsl");
	m_LightingPass = std::make_unique<Shader>("res/shaders/AdvancedLighting/DeferredShading/LightingPass.glsl");
	m_GBufferShaderTest = std::make_unique<Shader>("res/shaders/AdvancedLighting/DeferredShading/GBufferTest.glsl");

	m_Backpack = std::make_unique<Model>("res/meshes/Backpack/Backpack.gltf");
	m_Backpack->Finalize();
	m_ScreenQuad = std::make_unique<Plane2D>();
	m_LightVolume = std::make_unique<Sphere>();


	m_LightingPass->Bind();
	m_LightingPass->SetUniform1i("gPosition", 0);
	m_LightingPass->SetUniform1i("gNormal", 1);
	m_LightingPass->SetUniform1i("gAlbedoSpec", 2);
	
	m_GBufferShaderTest->Bind();
	m_GBufferShaderTest->SetUniform1i("gPosition", 0);
	m_GBufferShaderTest->SetUniform1i("gNormal", 1);
	m_GBufferShaderTest->SetUniform1i("gAlbedoSpec", 2);
	
	m_ObjectPositions =
	{
		glm::vec3(-3.0, -0.5, -3.0),
		glm::vec3( 0.0, -0.5, -3.0),
		glm::vec3( 3.0, -0.5, -3.0),
		glm::vec3(-3.0, -0.5,  0.0),
		glm::vec3( 0.0, -0.5,  0.0),
		glm::vec3( 3.0, -0.5,  0.0),
		glm::vec3(-3.0, -0.5,  3.0),
		glm::vec3( 0.0, -0.5,  3.0),
		glm::vec3( 3.0, -0.5,  3.0)
	};

	m_Light = std::make_unique<LightMarker>();

	const int NR_LIGHTS = 32;
	srand(static_cast<int>(time(0)));
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 2.0);
		float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		glm::vec3 pos(xPos, yPos, zPos);
		// also calculate random color
		float rColor = static_cast<float>(((rand() % 11) / 10.0f));
		float gColor = static_cast<float>(((rand() % 11) / 10.0f));
		float bColor = static_cast<float>(((rand() % 11) / 10.0f));
		glm::vec3 color(rColor, gColor, bColor);
		m_LightData.push_back({ pos, color });
	}

	m_SceneSelection[TEST] = "Test Scene";
	m_SceneSelection[NORMAL] = "Deferred Shading";
	m_CurrentSelection = NORMAL;
}

void DeferredShading::Render()
{
	m_GBuffer->Bind();
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
	glDisable(GL_BLEND);
	m_GeometryPass->Bind();
	m_GeometryPass->SetUniformMat4f("view", view);
	m_GeometryPass->SetUniformMat4f("projection", projection);
	for (int i = 0; i < m_ObjectPositions.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_ObjectPositions[i]);
		model = glm::scale(model, glm::vec3(0.5f));
		m_GeometryPass->SetUniformMat4f("model", model);
		m_Backpack->Draw(*m_GeometryPass, m_Context.Renderer);
	}
	m_GBuffer->Unbind();
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	switch (m_CurrentSelection)
	{
	case TEST:
		DrawDeferredTest();
		break;
	case NORMAL:
		DrawDeferredLighting();
		break;
	}
	
}

void DeferredShading::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Deferred Shading", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		if (ImGui::BeginCombo("Scene selection", m_SceneSelection[m_CurrentSelection].c_str()))
		{
			for (auto selection : m_SceneSelection)
			{
				bool isSelected = (m_CurrentSelection == selection.first);
				if (ImGui::Selectable(selection.second.c_str(), isSelected))
				{
					m_CurrentSelection = selection.first;
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::End();
}

void DeferredShading::DrawDeferredTest()
{
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);

	std::vector<glm::vec3> m_QuadPositions =
	{
		{-0.5,  0.5, 0.0},
		{ 0.5,  0.5, 0.0},
		{-0.5, -0.5, 0.0},
		{ 0.5, -0.5, 0.0}
	};
	for (int i = 0; i < m_QuadPositions.size(); i++)
	{
		m_Position->Bind(0);
		m_Normal->Bind(1);
		m_AlbedoSpec->Bind(2);
		
		glm::mat4 model(1.0);
		model = glm::translate(model, m_QuadPositions[i]);
		model = glm::scale(model, glm::vec3(0.5));
		m_GBufferShaderTest->Bind();
		m_GBufferShaderTest->SetUniformMat4f("model", model);
		m_GBufferShaderTest->SetUniform1i("Selection", i);
		m_ScreenQuad->Draw(*m_GBufferShaderTest, m_Context.Renderer);
	}
}

void DeferredShading::DrawDeferredLighting()
{
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	m_GBuffer->BindRead();
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);

	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	glm::mat4 model = glm::mat4(1.0);

	m_Position->Bind(0);
	m_Normal->Bind(1);
	m_AlbedoSpec->Bind(2);

	const float constant = 1.0f;
	const float linear = 0.7f;
	const float quadratic = 1.8f;

	glm::vec2 screenSize = { m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight() };

	m_LightingPass->Bind();
	m_LightingPass->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_LightingPass->SetUniformMat4f("projection", projection);
	m_LightingPass->SetUniformMat4f("view", view);
	m_LightingPass->SetUniformVec2("gScreenSize", screenSize);
	for (int i = 0; i < m_LightData.size(); i++)
	{
		float maxBrightness = std::fmaxf(std::fmaxf(m_LightData[i].second.r, m_LightData[i].second.g), m_LightData[i].second.b);

		float a, b, c;
		a = quadratic;
		b = linear;
		c = constant - maxBrightness * (256.0f / 5.0f);
		float radius = (-b + std::sqrt(b * b - 4 * a * c)) / (2.0f * a);
		m_LightingPass->SetUniform1f("radius", radius);
		model = glm::mat4(1.0);
		model = glm::translate(model, m_LightData[i].first);
		model = glm::scale(model, glm::vec3(radius));
		m_LightingPass->SetUniformMat4f("model", model);
		m_LightingPass->SetUniformVec3("light.Position", m_LightData[i].first);
		m_LightingPass->SetUniformVec3("light.Color", m_LightData[i].second);
		m_LightVolume->Draw(*m_LightingPass, m_Context.Renderer);
	}
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	DrawLights();
}

void DeferredShading::DrawLights()
{
	glEnable(GL_DEPTH_TEST);
	m_GBuffer->BlitBuffer(nullptr, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight(), GL_DEPTH_BUFFER_BIT);
	m_GBuffer->Unbind();
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	for (int i = 0; i < m_LightData.size(); i++)
	{
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightData[i].first, m_LightData[i].second);
	}
}
