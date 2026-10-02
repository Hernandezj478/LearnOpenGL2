#include "Framebuffers.h"

#include "Graphics/Buffers/FrameBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/ColorPalette.h"

#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Grid.h"

#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

Framebuffers::Framebuffers(const SceneContext& context) : Scene3D(context)
{
	srand((unsigned int)time(0));

	glEnable(GL_DEPTH_TEST);

	//Models
	m_Cube = std::make_unique<Cube>();
	m_Plane = std::make_unique<Plane>();
	m_ScreenPlane = std::make_unique<Plane2D>();
	m_MirrorPlane = std::make_unique<Plane2D>();

	m_Grid = std::make_unique<Grid>(50);

	//Shaders
	m_Shader			= std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Framebuffer/Framebuffer.glsl");
	m_GridShader		= std::make_unique<Shader>("res/shaders/GridLine/Line.glsl");
	m_KernelShader		= std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Framebuffer/Kernel.glsl");
	
	m_FrameShader		= std::make_shared<Shader>("res/shaders/AdvancedOpenGL/Framebuffer/FramebufferScreen.glsl");
	m_InvertedShader	= std::make_shared<Shader>("res/shaders/AdvancedOpenGL/Framebuffer/Invert.glsl");
	m_GreyscaleShader	= std::make_shared<Shader>("res/shaders/AdvancedOpenGL/Framebuffer/Greyscale.glsl");

	m_Shader->Bind();
	m_Shader->SetUniform1i("texture1", 0);
	m_FrameShader->Bind();
	m_FrameShader->SetUniform1i("screenTexture", 0);
	m_InvertedShader->Bind();
	m_InvertedShader->SetUniform1i("screenTexture", 0);
	m_GreyscaleShader->Bind();
	m_GreyscaleShader->SetUniform1i("screenTexture", 0);

	m_SelectedShader = m_FrameShader;

	// Textures
	m_CubeTexture = std::make_unique<Texture>("res/textures/container.jpg");
	m_PlaneTexture = std::make_unique<Texture>("res/textures/metal.png");
	m_GrassTexture = std::make_unique<Texture>("res/textures/grass_blades.png");
	m_GroundTexture = std::make_unique<Texture>("res/textures/grass.jpg");

	m_CubeTexture->SyncTexture();
	m_PlaneTexture->SyncTexture();
	m_GrassTexture->SyncTexture();
	m_GroundTexture->SyncTexture();

	//Framebuffer
	m_Framebuffer = std::make_unique<FrameBuffer>();
	m_Framebuffer->Bind();

	//Texturebuffers
	m_Colorbuffer = std::make_unique<RenderTexture>(1);
	m_Colorbuffer->CreateColorbuffer(m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_Framebuffer->AttachColorBuffer(*m_Colorbuffer);

	//Renderbuffer
	m_Renderbuffer = std::make_unique<RenderBuffer>();
	m_Renderbuffer->Bind();
	m_Renderbuffer->CreateStorage(GL_DEPTH24_STENCIL8, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());
	m_Framebuffer->AttachRenderBuffer(*m_Renderbuffer, GL_DEPTH_STENCIL_ATTACHMENT);
	m_Framebuffer->Unbind();

	m_Framebuffer->FrameBufferComplete();

	m_CubePositions =
	{
		{ 2.0f, 0.5f, 0.0f},
		{-2.0f, 0.5f, 0.0f}
	};
	for (int i = 0; i < m_GrassToRender; i++)
	{
		m_GrassPositions.push_back(glm::vec3(rand() % 10 - 5, 0.5f, rand() % 10 - 5));
	}
	m_Filter = std::make_unique<PostProcessFilters>();
	m_KernelSelection = KernelType::Sharpen;
	m_Filter->UpdateKernel(m_KernelSelection);

	KernelMap[KernelType::Sharpen] = "Sharpen";
	KernelMap[KernelType::Blur] = "Blur";
	KernelMap[KernelType::Edge] = "Edge";

	m_PostprocessMap.push_back("Normal");
	m_PostprocessMap.push_back("Greyscale");
	m_PostprocessMap.push_back("Inverted");

	m_BlurStrength = m_Filter->GetBlurStrength();
	m_TextureOffset = m_Filter->GetTextureOffset();
}

Framebuffers::~Framebuffers() = default;

void Framebuffers::Render()
{
	glViewport(0, 0, m_Colorbuffer->GetWidth(), m_Colorbuffer->GetHeight());
	if (bFilter)
	{
		m_KernelShader->Bind();
		m_KernelShader->SetUniform1f("textureOffset", m_Filter->GetTextureOffset());
		for (int i = 0; i < 9; i++)
		{
			m_KernelShader->SetUniform1f("kernel[" + std::to_string(i) + "]", m_Filter->GetKernelAt(i));
		}
	}
	// Render Here
	m_Framebuffer->Bind();
	glEnable(GL_DEPTH_TEST);
	m_Context.Renderer.Clear(DARK_GREY, COLOR_DEPTH);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);

	//Cubes
	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_CubePositions[i]);
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("model", model);
		m_CubeTexture->Bind();
		m_Cube->Draw(*m_Shader, m_Context.Renderer);
	}

	//Ground
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(m_GroundScale));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("model", model);
	m_GroundTexture->Bind();
	m_Plane->Draw(*m_Shader, m_Context.Renderer);
	m_GroundTexture->Unbind();

	//Grid
	model = glm::mat4(1.0f);

	m_GridShader->Bind();
	m_GridShader->SetUniformMat4f("view", view);
	m_GridShader->SetUniformMat4f("projection", projection);
	m_GridShader->SetUniformMat4f("model", model);
	m_GridShader->SetUniformVec3("cameraPos", m_Camera.GetPosition());
	m_GridShader->SetUniform1f("maxDistance", 50.0f);
	m_GridShader->SetUniform1f("minDistance", 1.0f);
	m_Grid->Draw(*m_GridShader, m_Context.Renderer);

	m_Framebuffer->Unbind();
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_Camera.GetScreenWidth(), m_Camera.GetScreenHeight());

	m_Context.Renderer.Clear(WHITE, COLOR);
	if (m_WireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_Colorbuffer->Bind();
	if (bFilter)
	{
		m_ScreenPlane->Draw(*m_KernelShader, m_Context.Renderer);
	}
	else
	{
		m_ScreenPlane->Draw(*m_SelectedShader, m_Context.Renderer);
	}
	if (m_WireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Framebuffers::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Framebuffer Testing", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		std::string wf = m_WireframeMode ? "ON" : "OFF";
		std::string label = "Wireframe Mode: " + wf;
		ImGui::Checkbox(label.c_str(), &m_WireframeMode);
		ImGui::Separator();
		if (ImGui::BeginCombo("Postprocess", m_PostprocessMap[m_PostprocessSelection].c_str()))
		{
			for (int i = 0; i < m_PostprocessMap.size(); i++)
			{
				bool isSelected = (m_PostprocessSelection == i);
				if (ImGui::Selectable(m_PostprocessMap[i].c_str(), isSelected))
				{
					m_PostprocessSelection = i;
					ChangePostProcess();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		ImGui::Checkbox("Apply Filter", &bFilter); 
		if (bFilter)
		{
			if (ImGui::BeginCombo("Filter", KernelMap[m_KernelSelection].c_str()))
			{
				for (auto it : KernelMap)
				{
					bool isSelected = (it.first == m_KernelSelection);
					if(ImGui::Selectable(it.second.c_str(), isSelected))
					{
						m_KernelSelection = it.first;
						ChangeFilter();
					}
				}
				ImGui::EndCombo();
			}
		}
	}
	ImGui::End();
}

void Framebuffers::ChangeFilter()
{
	m_Filter->UpdateKernel(m_KernelSelection);
	if (m_KernelSelection == KernelType::Blur)
	{
		m_Filter->UpdateTextureOffset(1.0f / 30.0f);
	}
	else
	{
		m_Filter->UpdateTextureOffset(1.0f / 300.f);
	}
}

void Framebuffers::ChangePostProcess()
{
	switch (m_PostprocessSelection)
	{
	case 0:
		// Normal
		m_SelectedShader = m_FrameShader;
		break;
	case 1:
		// Greyscale
		m_SelectedShader = m_GreyscaleShader;
		break;
	case 2:
		// Inverted
		m_SelectedShader = m_InvertedShader;
		break;
	}
}
