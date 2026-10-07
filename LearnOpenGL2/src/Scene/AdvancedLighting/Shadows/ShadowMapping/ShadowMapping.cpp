#include "ShadowMapping.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Buffers/Framebuffer.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>
#include <string>
ShadowMapping::ShadowMapping(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	m_Cube = std::make_unique<Cube>();
	m_ScreenQuad = std::make_unique<Plane2D>();
	m_Floor = std::make_unique<Plane>();
	m_FloorTexture = std::make_unique<Texture>("res/textures/wood.png");

	m_FloorTexture->SyncTexture();

	m_DepthmapFBO = std::make_unique<FrameBuffer>();
	m_DepthBuffer = std::make_unique<RenderTexture>(SHADOW_WIDTH, SHADOW_HEIGHT);
	m_DepthBuffer->Bind();
	m_DepthBuffer->SetWrapS(GL_CLAMP_TO_BORDER);
	m_DepthBuffer->SetWrapT(GL_CLAMP_TO_BORDER);
	m_DepthBuffer->CreateTextureBuffer(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
	m_DepthBuffer->CreateBorder(WHITE);

	m_DepthmapFBO->Bind();
	m_DepthmapFBO->AttachDepthBuffer(*m_DepthBuffer);
	m_DepthmapFBO->NoColorData();
	m_DepthmapFBO->Unbind();

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/Shadows/ShadowMapping/ShadowMapping.glsl");
	m_DepthShader = std::make_unique<Shader>("res/shaders/AdvancedLighting/Shadows/ShadowMapping/ShadowMappingDepth.glsl");
	m_DepthDebug = std::make_unique<Shader>("res/shaders/AdvancedLighting/Shadows/ShadowMapping/DebugQuad.glsl");

	m_Shader->Bind();
	m_Shader->SetUniform1i("diffuseTexture", 0);
	m_Shader->SetUniform1i("shadowMap", 1);

	m_DepthDebug->Bind();
	m_DepthDebug->SetUniform1i("depthMap", 0);

	m_CubePositions =
	{
		glm::vec3{ 0.0f, 1.5f, 0.0},
		glm::vec3{ 2.0f, 0.0f, 1.0},
		glm::vec3{-1.0f, 0.0f, 2.0}
	};

	m_CubeScales =
	{
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(0.25f)
	};

	m_LightPosition = glm::vec3(-2.0f, 4.0f, -1.0f);

	m_Light = std::make_unique<LightMarker>();
}

void ShadowMapping::Render()
{
	// 1. Render to depth
	glCullFace(GL_FRONT);
	float near = 1.0f, far = 7.5f;
	glm::mat4 lightProjection = glm::mat4(1.0f);
	if (m_UseOrtho)
	{
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);

	}
	else
	{
		lightProjection = glm::perspective(90.0f, 1.7f, near, far);
	}
	glm::mat4 lightView = glm::lookAt(m_LightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	m_DepthShader->Bind();
	m_DepthShader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	m_DepthmapFBO->Bind();
	m_FloorTexture->Bind();
	m_Context.Renderer.ClearBufferBits(DEPTH);

	//Floor
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(25.0f));
	m_DepthShader->SetUniformMat4f("model", model);
	m_Floor->Draw(*m_DepthShader, m_Context.Renderer);

	//Cubes
	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, m_CubePositions[i]);
		model = glm::scale(model, m_CubeScales[i]);
		if (i == 2)
		{
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		}
		m_DepthShader->SetUniformMat4f("model", model);
		m_DepthShader->Bind();
		m_Cube->Draw(*m_DepthShader, m_Context.Renderer);
	}

	m_DepthmapFBO->Unbind();
	glCullFace(GL_BACK);
	// reset viewport
	glViewport(0, 0, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
	
	//2. Render as normal
	m_Shader->Bind();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 100.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	// set light uniforms
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_Shader->SetUniformVec3("lightPos", m_LightPosition);
	m_Shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
	m_FloorTexture->Bind();
	m_DepthBuffer->Bind(1);
	
	//Floor
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(25.0f));
	m_Shader->SetUniformMat4f("model", model);
	m_Shader->SetUniform1f("uvScale", 10.0f);
	m_Floor->Draw(*m_Shader, m_Context.Renderer);
	
	//Cubes
	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, m_CubePositions[i]);
		model = glm::scale(model, m_CubeScales[i]);
		if (i == 2)
		{
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		}
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("model", model);
		m_Shader->SetUniform1f("uvScale", 1.0f);
		m_Cube->Draw(*m_Shader, m_Context.Renderer);
	}

	m_Light->Draw(m_Context.Renderer, view, projection, m_LightPosition, WHITE);

	// Render depth map to quad for debugging
	if (m_DebugMode)
	{
		m_DepthDebug->Bind();
		m_DepthBuffer->Bind();
		m_DepthDebug->SetUniform1f("near", near);
		m_DepthDebug->SetUniform1f("far", far);
		m_ScreenQuad->Draw(*m_DepthDebug, m_Context.Renderer);
	}
}

void ShadowMapping::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Shadow Mapping", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		std::string s = m_UseOrtho ? "Orthographic" : "Perspective";
		std::string projLabel = "Projection: " + s;
		ImGui::Text(projLabel.c_str());
		ImGui::Checkbox("Use Orthographic Perspective", &m_UseOrtho);
		ImGui::Separator();
		ImGui::Checkbox("Debug Depth Mode", &m_DebugMode);
	}
	ImGui::End();
}
