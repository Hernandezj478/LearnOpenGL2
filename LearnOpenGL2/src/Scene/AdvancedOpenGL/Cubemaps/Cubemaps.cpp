#include "Cubemaps.h"

#include "Core/Input.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Cubemap.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Grid.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Skybox.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

Cubemaps::Cubemaps(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	m_SkyboxFilepaths =
	{
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg",
	};

	//Models
	m_Cube = std::make_unique<Cube>();
	m_Skybox = std::make_unique<Skybox>();

	m_Plane = std::make_unique<Plane>();
	m_Grid = std::make_unique<Grid>(100);

	std::string modelFilepath = "res/meshes/Backpack/Backpack.gltf";
	/*m_Loader.Request([modelFilepath]() -> std::unique_ptr<Model>
		{
			return std::make_unique<Model>(modelFilepath);
		});*/

	//Shaders
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Framebuffer/Framebuffer.glsl");
	m_SkyboxShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Cubemap/Skybox.glsl");
	m_GridShader = std::make_unique<Shader>("res/shaders/GridLine/Line.glsl");
	m_ReflectionShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Cubemap/Reflection.glsl");
	m_RefractionShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/Cubemap/Refraction.glsl");

	m_Shader->Bind();
	m_Shader->SetUniform1i("texture1", 0);

	m_SkyboxShader->Bind();
	m_SkyboxShader->SetUniform1i("skybox", 0);

	m_ReflectionShader->Bind();
	m_ReflectionShader->SetUniform1i("skybox", 0);

	m_RefractionShader->Bind();
	m_RefractionShader->SetUniform1i("skybox", 0);

	// Textures
	m_CubeTexture = std::make_unique<Texture>("res/textures/container.jpg");
	m_PlaneTexture = std::make_unique<Texture>("res/textures/metal.png");
	m_GroundTexture = std::make_unique<Texture>("res/textures/grass.jpg");

	m_CubeTexture->SyncTexture();
	m_PlaneTexture->SyncTexture();
	m_GroundTexture->SyncTexture();

	m_Cubemap = std::make_unique<Cubemap>(m_SkyboxFilepaths);

	m_CubePositions =
	{
		{ 2.0f, 0.5f, 0.0f},
		{-2.0f, 0.5f, 0.0f}
	};
}

Cubemaps::~Cubemaps()
{}

void Cubemaps::Render()
{
	if (bWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (!bWireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Render Here
	m_Context.Renderer.Clear(DARK_GREY, COLOR_DEPTH);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 1000.0f);

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);

	m_GridShader->Bind();
	m_GridShader->SetUniformMat4f("projection", projection);
	m_GridShader->SetUniformMat4f("view", view);

	m_ReflectionShader->Bind();
	m_ReflectionShader->SetUniformMat4f("projection", projection);
	m_ReflectionShader->SetUniformMat4f("view", view);
	m_ReflectionShader->SetUniformVec3("cameraPos", m_Camera.GetPosition());

	m_RefractionShader->Bind();
	m_RefractionShader->SetUniformMat4f("projection", projection);
	m_RefractionShader->SetUniformMat4f("view", view);
	m_RefractionShader->SetUniformVec3("cameraPos", m_Camera.GetPosition());

	//Cubes
	for (int i = 0; i < m_CubePositions.size(); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, m_CubePositions[i]);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		m_ReflectionShader->Bind();
		m_ReflectionShader->SetUniformMat4f("model", model);
		m_CubeTexture->Bind();
		m_Cube->Draw(*m_ReflectionShader, m_Context.Renderer);
	}

	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.25f));
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("model", model);
	for (int i = 0; i < m_Models.size(); i++)
	{
		m_Models[i]->Draw(*m_Shader, m_Context.Renderer);
	}

	//Ground
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(m_GroudScale));
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

	//Skybox
	glDepthFunc(GL_LEQUAL);
	m_SkyboxShader->Bind();
	m_SkyboxShader->SetUniformMat4f("projection", projection);
	view = glm::mat4(glm::mat3(m_Camera.GetViewMatrix()));
	model = glm::mat4(1.0f);
	m_SkyboxShader->SetUniformMat4f("view", view);
	m_SkyboxShader->SetUniformMat4f("model", model);
	m_Cubemap->Bind();
	m_Skybox->Draw(*m_SkyboxShader, m_Context.Renderer);
	m_SkyboxShader->Unbind();
	glDepthFunc(GL_LESS);
}

void Cubemaps::Update(float deltaTime)
{
	Scene3D::Update(deltaTime);
	m_Loader.Update(
		[this](std::unique_ptr<Model> model)
		{
			model->Finalize();
			m_Models.push_back(std::move(model));
		},
		[this](const std::exception& e)
		{
			m_LastLoadError = e.what();
		});
	if (m_Context.Input.IsPressed(GLFW_KEY_F1))
	{
		bWireframeMode = !bWireframeMode;
	}
}

void Cubemaps::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Cubemaps", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::Text("F1 Wireframe Mode");
		ImGui::Checkbox("Wireframe", &bWireframeMode);
	}
	ImGui::End();
}
