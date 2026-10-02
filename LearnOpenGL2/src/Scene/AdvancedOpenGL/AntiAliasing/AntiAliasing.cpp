#include "AntiAliasing.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/Framebuffer.h"
#include "Graphics/Buffers/Renderbuffer.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"

#include "Scene/SceneContext.h"


AntiAliasing::AntiAliasing(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	m_Cube = std::make_unique<Cube>();
	m_ScreenPlane = std::make_unique<Plane2D>();
	m_CubeTexture = std::make_unique<Texture>("res/textures/container2.png");
	m_CubeTexture->SyncTexture();

	m_Framebuffer = std::make_unique<FrameBuffer>();
	m_RenderTexture = std::make_unique<RenderTexture>();
	m_RenderTexture->CreateColorBufferMSAA(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight(), 4);
	m_RenderTexture->Unbind();

	m_Framebuffer->Bind();
	m_Framebuffer->AttachColorBuffer(*m_RenderTexture);
	
	m_Renderbuffer = std::make_unique<RenderBuffer>();
	m_Renderbuffer->CreateStorageMultiSample(GL_DEPTH24_STENCIL8, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight(), 4);
	m_Renderbuffer->Unbind();
	m_Renderbuffer->Bind();
	
	m_Framebuffer->AttachRenderBuffer(*m_Renderbuffer, GL_DEPTH_STENCIL_ATTACHMENT);
	m_Framebuffer->Unbind();

	m_IFB = std::make_unique<FrameBuffer>();
	m_ScreenTexture = std::make_unique<RenderTexture>();
	m_ScreenTexture->CreateColorbuffer(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_IFB->Bind();
	m_IFB->AttachColorBuffer(*m_ScreenTexture);
	m_IFB->Unbind();

	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/AntiAliasing/BaseShader.glsl");
	m_ScreenShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/AntiAliasing/AntiAliasing.glsl");

	m_Shader->Bind();
	m_Shader->SetUniform1i("texture1", 0);

	m_ScreenShader->Bind();
	m_ScreenShader->SetUniform1i("screenTexture", 0);
}

void AntiAliasing::Render()
{
	m_Framebuffer->Bind();
	m_Context.Renderer.Clear(DARK_GREY, COLOR_DEPTH);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 1000.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("model", model);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformMat4f("projection", projection);
	m_CubeTexture->Bind();
	m_Cube->Draw(*m_Shader, m_Context.Renderer);
	m_CubeTexture->Unbind();

	m_Framebuffer->BlitColor(*m_IFB, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_Framebuffer->Unbind();

	glDisable(GL_DEPTH_TEST);
	m_Context.Renderer.Clear(WHITE, COLOR);

	m_ScreenShader->Bind();
	m_ScreenTexture->Bind();
	m_ScreenPlane->Draw(*m_ScreenShader, m_Context.Renderer);
	glEnable(GL_DEPTH_TEST);
}
