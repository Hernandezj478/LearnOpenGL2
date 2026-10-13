#include "SpecularIBL.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Cubemap.h"
#include "Graphics/LightMarker.h"
#include "Graphics/Renderer.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/Framebuffer.h"
#include "Graphics/Buffers/RenderBuffer.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Shapes/Cube.h"
#include "Graphics/Shapes/Plane.h"
#include "Graphics/Shapes/Sphere.h"
#include "Graphics/Shapes/Skybox.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

SpecularIBL::SpecularIBL(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glDepthFunc(GL_LEQUAL);

	m_Cube = std::make_unique<Cube>();
	m_Sphere = std::make_unique<Sphere>();
	m_Light = std::make_unique<LightMarker>();
	m_Skybox = std::make_unique<Skybox>();
	m_ScreenQuad = std::make_unique<Plane2D>();

	m_Backpack = std::make_unique<Model>("res/meshes/Backpack/Backpack.gltf");
	m_Backpack->Finalize();

	m_LoftRoom = std::make_unique<Texture>("res/textures/HDR/newport_loft.hdr");
	m_LoftRoom->SetHDR(true);
	m_LoftRoom->SyncTexture();

	InitializeSphereTexutres();

	m_PBRDiffuse = std::make_unique<Shader>("res/shaders/PBR/IBL/SpecularIBL/IBLDiffuse.glsl");
	m_PBRDiffuse->Bind();
	m_PBRDiffuse->SetUniformVec3("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
	m_PBRDiffuse->SetUniform1f("ao", 1.0f);
	m_PBRDiffuse->SetUniform1i("irradianceMap", 0);
	m_PBRDiffuse->SetUniform1i("prefilteredMap", 1);
	m_PBRDiffuse->SetUniform1i("brdfLUT", 2);

	m_PBRTextured = std::make_unique<Shader>("res/shaders/PBR/IBL/SpecularIBL/IBLTextured.glsl");
	m_PBRTextured->Bind();
	m_PBRTextured->SetUniform1i("irradianceMap", 0);
	m_PBRTextured->SetUniform1i("prefilteredMap", 1);
	m_PBRTextured->SetUniform1i("brdfLUT", 2);
	m_PBRTextured->SetUniform1i("albedoMap", 3);
	m_PBRTextured->SetUniform1i("normalMap", 4);
	m_PBRTextured->SetUniform1i("metallicMap", 5);
	m_PBRTextured->SetUniform1i("roughnessMap", 6);
	m_PBRTextured->SetUniform1i("aoMap", 7);

	m_PBRModel = std::make_unique<Shader>("res/shaders/PBR/IBL/SpecularIBL/IBLModel.glsl");
	m_PBRModel->Bind();
	m_PBRModel->SetUniform1i("irradianceMap", 0);
	m_PBRModel->SetUniform1i("prefilteredMap", 1);
	m_PBRModel->SetUniform1i("brdfLUT", 2);


	m_BRDFShader = std::make_unique<Shader>("res/shaders/PBR/IBL/SpecularIBL/BRDF.glsl");

	m_EquiRectMap = std::make_unique<Shader>("res/shaders/PBR/IBL/DiffuseIrradiance/EquiRectMap.glsl");
	m_EquiRectMap->Bind();
	m_EquiRectMap->SetUniform1i("equirectMap", 0);

	m_EnvShader = std::make_unique<Shader>("res/shaders/PBR/IBL/DiffuseIrradiance/SimpleSkybox.glsl");
	m_EnvShader->Bind();
	m_EnvShader->SetUniform1i("envMap", 0);

	m_IrradianceMap = std::make_unique<Shader>("res/shaders/PBR/IBL/DiffuseIrradiance/CubemapConvolution.glsl");
	m_IrradianceMap->Bind();
	m_IrradianceMap->SetUniform1i("envMap", 0);

	m_PrefilterMap = std::make_unique<Shader>("res/shaders/PBR/IBL/SpecularIBL/PrefilterSpecular.glsl");
	m_PrefilterMap->Bind();
	m_PrefilterMap->SetUniform1i("envMap", 0);

	m_captureFBO = std::make_unique<FrameBuffer>();
	m_captureRBO = std::make_unique<RenderBuffer>();

	m_envCubemap = std::make_unique<Cubemap>();
	m_envCubemap->SetMagFilter(GL_LINEAR);
	m_envCubemap->SetMinFilter(GL_LINEAR);
	m_envCubemap->CreateHDRCubemap(512, 512);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	m_captureFBO->Bind();
	m_captureRBO->Bind();
	m_captureRBO->CreateStorage(GL_DEPTH_COMPONENT24, 512, 512);
	m_captureFBO->AttachRenderBuffer(*m_captureRBO, GL_DEPTH_ATTACHMENT);
	m_captureFBO->FrameBufferComplete();

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
	m_captureFBO->FrameBufferComplete();
	m_captureFBO->Unbind();

	m_PrefilterCubemap = std::make_unique<Cubemap>();
	m_PrefilterCubemap->SetMagFilter(GL_LINEAR);
	m_PrefilterCubemap->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	m_PrefilterCubemap->CreateHDRCubemap(128, 128);
	m_PrefilterCubemap->GenerateMipmap();

	m_PrefilterMap->Bind();
	m_PrefilterMap->SetUniformMat4f("projection", captureProjection);
	m_envCubemap->Bind();

	m_captureFBO->Bind();
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; mip++)
	{
		unsigned int mipWidth  = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		m_captureRBO->Bind();
		m_captureRBO->CreateStorage(GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		m_PrefilterMap->SetUniform1f("roughness", roughness);
		for (unsigned int i = 0; i < 6; i++)
		{
			m_PrefilterMap->SetUniformMat4f("view", captureViews[i]);
			m_captureFBO->AttachCubemap(*m_PrefilterCubemap, i, 0, mip);
			m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
			m_Skybox->Draw(*m_PrefilterMap, m_Context.Renderer);
		}
	}
	m_captureFBO->FrameBufferComplete();
	m_captureFBO->Unbind();

	m_brdfLUT = std::make_unique<RenderTexture>(512, 512);
	m_brdfLUT->Bind();
	m_brdfLUT->SetWrapS(GL_CLAMP_TO_EDGE);
	m_brdfLUT->SetWrapT(GL_CLAMP_TO_EDGE);
	m_brdfLUT->SetMinFilter(GL_LINEAR);
	m_brdfLUT->SetMagFilter(GL_LINEAR);
	m_brdfLUT->CreateTextureBuffer(GL_RG16F, GL_RG, GL_FLOAT);

	m_captureFBO->Bind();
	m_captureRBO->Bind();
	m_captureRBO->CreateStorage(GL_DEPTH_COMPONENT24, 512, 512);
	m_captureFBO->AttachColorBuffer(*m_brdfLUT);
	m_captureFBO->FrameBufferComplete();

	glViewport(0, 0, 512, 512);
	m_BRDFShader->Bind();
	m_Context.Renderer.ClearBufferBits(COLOR_DEPTH);
	m_ScreenQuad->Draw(*m_BRDFShader, m_Context.Renderer);
	m_captureFBO->Unbind();

	m_LightData.push_back({ glm::vec3(-10.0f,  10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3(10.0f,  10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });
	m_LightData.push_back({ glm::vec3(10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f) });

	m_SceneMap[BASIC] = "Basic Scene";
	m_SceneMap[TEXTURED] = "Textured Spheres";
	m_SceneMap[MODEL] = "Model Scene";
}

void SpecularIBL::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);
	
	switch (m_SelectedScene)
	{
	case BASIC:
		RenderBasicScene();
		break;
	case TEXTURED:
		RenderTexturedScene();
		break;
	case MODEL:
		RenderModelScene();
	}


	m_EnvShader->Bind();
	m_EnvShader->SetUniformMat4f("projection", projection);
	m_EnvShader->SetUniformMat4f("view", view);
	m_envCubemap->Bind();
	m_Skybox->Draw(*m_EnvShader, m_Context.Renderer);
}

void SpecularIBL::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Specular IBL", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		if(ImGui::BeginCombo("Select Scene", m_SceneMap[m_SelectedScene].c_str()))
		{
			for (auto scene : m_SceneMap)
			{
				bool selected = (m_SelectedScene == scene.first);
				if (ImGui::Selectable(scene.second.c_str(), selected))
				{
					m_SelectedScene = scene.first;
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::End();
}

void SpecularIBL::InitializeSphereTexutres()
{
	std::map<std::string, std::shared_ptr<Texture>> Gold;
	std::map<std::string, std::shared_ptr<Texture>> Grass;
	std::map<std::string, std::shared_ptr<Texture>> Plastic;
	std::map<std::string, std::shared_ptr<Texture>> Iron;
	std::map<std::string, std::shared_ptr<Texture>> Wall;

	Gold["albedo"]	= std::make_shared<Texture>("res/textures/PBR/Gold/albedo.png");
	Gold["normal"]	= std::make_shared<Texture>("res/textures/PBR/Gold/normal.png");
	Gold["metallic"] = std::make_shared<Texture>("res/textures/PBR/Gold/metallic.png");
	Gold["roughness"] = std::make_shared<Texture>("res/textures/PBR/Gold/roughness.png");
	Gold["ao"] = std::make_shared<Texture>("res/textures/PBR/Gold/ao.png");

	Grass["albedo"] = std::make_shared<Texture>("res/textures/PBR/Grass/albedo.png");
	Grass["normal"] = std::make_shared<Texture>("res/textures/PBR/Grass/normal.png");
	Grass["metallic"] = std::make_shared<Texture>("res/textures/PBR/Grass/metallic.png");
	Grass["roughness"] = std::make_shared<Texture>("res/textures/PBR/Grass/roughness.png");
	Grass["ao"] = std::make_shared<Texture>("res/textures/PBR/Grass/ao.png");

	Plastic["albedo"] = std::make_shared<Texture>("res/textures/PBR/Plastic/albedo.png");
	Plastic["normal"] = std::make_shared<Texture>("res/textures/PBR/Plastic/normal.png");
	Plastic["metallic"] = std::make_shared<Texture>("res/textures/PBR/Plastic/metallic.png");
	Plastic["roughness"] = std::make_shared<Texture>("res/textures/PBR/Plastic/roughness.png");
	Plastic["ao"] = std::make_shared<Texture>("res/textures/PBR/Plastic/ao.png");

	Iron["albedo"] = std::make_shared<Texture>("res/textures/PBR/Rusted_Iron/albedo.png");
	Iron["normal"] = std::make_shared<Texture>("res/textures/PBR/Rusted_Iron/normal.png");
	Iron["metallic"] = std::make_shared<Texture>("res/textures/PBR/Rusted_Iron/metallic.png");
	Iron["roughness"] = std::make_shared<Texture>("res/textures/PBR/Rusted_Iron/roughness.png");
	Iron["ao"] = std::make_shared<Texture>("res/textures/PBR/Rusted_Iron/ao.png");

	Wall["albedo"] = std::make_shared<Texture>("res/textures/PBR/Wall/albedo.png");
	Wall["normal"] = std::make_shared<Texture>("res/textures/PBR/Wall/normal.png");
	Wall["metallic"] = std::make_shared<Texture>("res/textures/PBR/Wall/metallic.png");
	Wall["roughness"] = std::make_shared<Texture>("res/textures/PBR/Wall/roughness.png");
	Wall["ao"] = std::make_shared<Texture>("res/textures/PBR/Wall/ao.png");

	m_SphereTextures.push_back(Gold);
	m_SphereTextures.push_back(Grass);
	m_SphereTextures.push_back(Plastic);
	m_SphereTextures.push_back(Iron);
	m_SphereTextures.push_back(Wall);


	for (int i = 0; i < m_SphereTextures.size(); i++)
	{
		//m_SphereTextures[i].at("albedo")->SetHDR(true);
		m_SphereTextures[i].at("albedo")->SetFlipImage(false);
		//m_SphereTextures[i].at("albedo")->SetGammaCorrection(true);
		m_SphereTextures[i].at("albedo")->SyncTexture();

		//m_SphereTextures[i].at("normal")->SetHDR(true);
		m_SphereTextures[i].at("normal")->SetFlipImage(false);
		//m_SphereTextures[i].at("normal")->SetGammaCorrection(true);
		m_SphereTextures[i].at("normal")->SyncTexture();
		
		//m_SphereTextures[i].at("metallic")->SetHDR(true);
		m_SphereTextures[i].at("metallic")->SetFlipImage(false);
		//m_SphereTextures[i].at("metallic")->SetGammaCorrection(true);
		m_SphereTextures[i].at("metallic")->SyncTexture();
		
		//m_SphereTextures[i].at("roughness")->SetHDR(true);
		m_SphereTextures[i].at("roughness")->SetFlipImage(false);
		//m_SphereTextures[i].at("roughness")->SetGammaCorrection(true);
		m_SphereTextures[i].at("roughness")->SyncTexture();
		
		//m_SphereTextures[i].at("ao")->SetHDR(true);
		m_SphereTextures[i].at("ao")->SetFlipImage(false);
		//m_SphereTextures[i].at("ao")->SetGammaCorrection(true);
		m_SphereTextures[i].at("ao")->SyncTexture();
	}
}

void SpecularIBL::RenderBasicScene()
{
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_PBRDiffuse->Bind();
	m_PBRDiffuse->SetUniformMat4f("projection", projection);
	m_PBRDiffuse->SetUniformMat4f("view", view);
	m_PBRDiffuse->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_PBRDiffuse->SetUniform1i("bGammaCorrect", m_UseGamma);
	m_IrradianceCubemap->Bind(0);
	m_PrefilterCubemap->Bind(1);
	m_brdfLUT->Bind(2);
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
}

void SpecularIBL::RenderTexturedScene()
{
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_PBRTextured->Bind();
	m_PBRTextured->SetUniformMat4f("projection", projection);
	m_PBRTextured->SetUniformMat4f("view", view);
	m_PBRTextured->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_PBRTextured->SetUniform1i("bGammaCorrect", m_UseGamma);
	m_IrradianceCubemap->Bind(0);
	m_PrefilterCubemap->Bind(1);
	m_brdfLUT->Bind(2);
	for (int i = 0; i < m_SphereTextures.size(); i++)
	{
		m_SphereTextures[i].at("albedo")->Bind(3);
		m_SphereTextures[i].at("normal")->Bind(4);
		m_SphereTextures[i].at("metallic")->Bind(5);
		m_SphereTextures[i].at("roughness")->Bind(6);
		m_SphereTextures[i].at("ao")->Bind(7);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(i * m_Spacing, 0.0f, 0.0f));
		m_PBRTextured->SetUniformMat4f("model", model);
		m_Sphere->Draw(*m_PBRTextured, m_Context.Renderer);
	}
	for (int i = 0; i < m_LightData.size(); i++)
	{
		m_PBRTextured->Bind();
		m_PBRTextured->SetUniformVec3("light[" + std::to_string(i) + "].Position", m_LightData[i].first);
		m_PBRTextured->SetUniformVec3("light[" + std::to_string(i) + "].Color", m_LightData[i].second);
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightData[i].first, m_LightData[i].second);
	}
}

void SpecularIBL::RenderModelScene()
{
	glm::mat4 model(1.0);
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_PBRModel->Bind();
	m_PBRModel->SetUniformMat4f("projection", projection);
	m_PBRModel->SetUniformMat4f("view", view);
	m_PBRModel->SetUniformVec3("viewPos", m_Camera.GetPosition());
	m_PBRModel->SetUniform1i("bGammaCorrect", m_UseGamma);
	m_IrradianceCubemap->Bind(0);
	m_PrefilterCubemap->Bind(1);
	m_brdfLUT->Bind(2);
	m_PBRModel->SetUniformMat4f("model", model);
	// We have to make sure to initialize the textures in GLSL so that these dont override the other texture units.
	// When any sampler2D textures go unused, they are automatically bound to unit 0, which will conflict with whatever
	// uses unit 0 - in this case the samplerCube IrradianceCubemap. Best way to approach this if we are rendering
	// models with PBR, set up pre-calculated maps at higher unit bindings.
	for (int i = 0; i < 16; i++)
	{
		m_PBRModel->SetUniform1i("textures[" + std::to_string(i) + "]", i + 3);
	}

	m_Backpack->SetTextureOffset(3);
	m_Backpack->Draw(*m_PBRModel, m_Context.Renderer);

	for (int i = 0; i < m_LightData.size(); i++)
	{
		m_PBRModel->Bind();
		m_PBRModel->SetUniformVec3("light[" + std::to_string(i) + "].Position", m_LightData[i].first);
		m_PBRModel->SetUniformVec3("light[" + std::to_string(i) + "].Color", m_LightData[i].second);
		m_Light->Draw(m_Context.Renderer, view, projection, m_LightData[i].first, m_LightData[i].second);
	}
}
