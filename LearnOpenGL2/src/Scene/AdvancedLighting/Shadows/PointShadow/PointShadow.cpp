#include "PointShadow.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Cubemap.h"
#include "Graphics/Buffers/Framebuffer.h"
#include "Graphics/Shapes/Cube.h"
#include "Scene/SceneContext.h"

PointShadow::PointShadow(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);

	m_ShadowTransformTarget =
	{
		glm::vec3(1.0,  0.0,  0.0),
		glm::vec3(-1.0,  0.0,  0.0),
		glm::vec3(0.0,  1.0,  0.0),
		glm::vec3(0.0, -1.0,  0.0),
		glm::vec3(0.0,  0.0,  1.0),
		glm::vec3(0.0,  0.0, -1.0),
	};

	m_ShadowUpVectors =
	{
		glm::vec3(0.0, -1.0,  0.0),
		glm::vec3(0.0, -1.0,  0.0),
		glm::vec3(0.0,  0.0,  1.0),
		glm::vec3(0.0,  0.0, -1.0),
		glm::vec3(0.0, -1.0,  0.0),
		glm::vec3(0.0, -1.0,  0.0),
	};

	m_CubePositions =
	{
		glm::vec3(4.0f, -3.5f, 0.0),
		glm::vec3(2.0f, 3.0f, 1.0),
		glm::vec3(-3.0f, -1.0f, 0.0),
		glm::vec3(-1.5f, 1.0f, 1.5),
		glm::vec3(-1.5f, 2.0f, -3.0),
	};

	m_CubeScales =
	{
		glm::vec3(0.5f),
		glm::vec3(0.75f),
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(0.75f),
	};
	m_Cube = std::make_unique<Cube>();
	m_Light = std::make_unique<LightMarker>();

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/Shadows/PointShadow/PointShadow.glsl");
	m_DepthShader = std::make_unique<Shader>("res/shaders/AdvancedLighting/Shadows/PointShadow/DepthMapping.glsl");

	m_WoodTexture = std::make_unique<Texture>("res/textures/wood.png");
	m_WoodTexture->SyncTexture();

	m_DepthCubeMap = std::make_unique<Cubemap>();
	m_DepthCubeMap->CreateDepthCubeMap(SHADOW_WIDTH, SHADOW_HEIGHT);

	m_DepthMapFBO = std::make_unique<FrameBuffer>();
	m_DepthMapFBO->Bind();
	m_DepthMapFBO->AttachDepthCubeMap(*m_DepthCubeMap);
	m_DepthMapFBO->NoColorData();
	m_DepthMapFBO->FrameBufferComplete();
	m_DepthMapFBO->Unbind();

	m_Shader->Bind();
	m_Shader->SetUniform1i("diffuseTexture", 0);
	m_Shader->SetUniform1i("depthMap", 1);
	m_Shader->Unbind();
}

void PointShadow::Render()
{
	m_LightPos.x = static_cast<float>(cos(glfwGetTime() * 0.5) * 3.0);
	m_LightPos.z = static_cast<float>(sin(glfwGetTime() * 0.5) * 3.0);
	m_Context.Renderer.Clear(DARK_GREY, COLOR_DEPTH);
	// create cubemap transform matrices
	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float nearPlane = 0.1f;
	float far_plane = 20.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, nearPlane, far_plane);
	std::vector<glm::mat4> shadowTransforms;

	for (int i = 0; i < m_ShadowTransformTarget.size(); ++i)
	{
		glm::mat4 transform =
			shadowProj * glm::lookAt(m_LightPos, m_LightPos + m_ShadowTransformTarget[i], m_ShadowUpVectors[i]);
		shadowTransforms.push_back(transform);
	}

	// Render Here
	// render scene to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	m_DepthMapFBO->Bind();
	m_Context.Renderer.ClearBufferBits(DEPTH);
	m_DepthShader->Bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		m_DepthShader->SetUniformMat4f("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}
	m_DepthShader->SetUniform1f("far_plane", far_plane);
	m_DepthShader->SetUniformVec3("lightPos", m_LightPos);
	m_DepthCubeMap->Bind(1);
	// Render scene
	RenderScene(*m_DepthShader);

	m_DepthMapFBO->Unbind();
	m_DepthShader->Unbind();
	m_DepthCubeMap->Unbind();

	// render scene as normal
	glViewport(0, 0, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
	
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 100.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	// set lighting uniforms
	m_Shader->SetUniformVec3("lightPos", m_LightPos);
	m_Shader->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_Shader->SetUniform1f("far_plane", far_plane);
	m_Shader->SetUniformVec3("lightColor", WHITE);
	m_WoodTexture->Bind(0);
	m_DepthCubeMap->Bind(1);
	
	// Render scene
	RenderScene(*m_Shader);

	m_Shader->Unbind();
}

void PointShadow::RenderScene(Shader& Shader)
{
	// skybox
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 100.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	model = glm::scale(model, glm::vec3(10.0f));
	Shader.Bind();
	Shader.SetUniformMat4f("model", model);
	glDisable(GL_CULL_FACE);
	Shader.SetUniform1i("reverse_normals", 1);
	m_Cube->Draw(Shader, m_Context.Renderer);
	Shader.SetUniform1i("reverse_normals", 0);
	glEnable(GL_CULL_FACE);
	// lightbox
	m_Light->Draw(m_Context.Renderer, view, projection, m_LightPos, WHITE);
	//cubes
	for (int i = 0; i < 5; ++i)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, m_CubePositions[i]);
		if (i == 4)
		{
			model = glm::rotate(model, glm::radians(6.0f), glm::vec3(-1.5f, 2.0f, -3.0f));
		}
		model = glm::scale(model, m_CubeScales[i]);
		Shader.SetUniformMat4f("model", model);
		m_Cube->Draw(Shader, m_Context.Renderer);
	}
}
