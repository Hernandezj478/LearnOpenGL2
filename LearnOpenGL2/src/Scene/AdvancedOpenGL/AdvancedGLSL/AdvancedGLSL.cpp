#include "AdvancedGLSL.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/UniformBuffer.h"
#include "Graphics/Shapes/Cube.h"
#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

AdvancedGLSL::AdvancedGLSL(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_DEPTH_TEST);
	m_Cube = std::make_unique<Cube>();

	m_PointShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/AdvancedGLSL/PointSize.glsl");
	m_CoordShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/AdvancedGLSL/FragCoord.glsl");
	m_FrontFacingShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/AdvancedGLSL/FrontFacing.glsl");
	m_ColorCubesShader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/AdvancedGLSL/UB_Colors.glsl");

	m_FaceTexture = std::make_unique<Texture>("res/textures/GreyboxTexture.png");
	m_BackTexture = std::make_unique<Texture>("res/textures/Tile.jpg");

	m_FaceTexture->SyncTexture();
	m_BackTexture->SyncTexture();

	m_UBO = std::make_unique<UniformBuffer>("Matrices", 0);
	m_UBO->Bind();
	m_UBO->CreateBufferObject(nullptr, 2 * sizeof(glm::mat4));
	m_UBO->BindBase();

	m_UBO->AddShader(*m_PointShader);
	m_UBO->AddShader(*m_CoordShader);
	m_UBO->AddShader(*m_ColorCubesShader);
	

	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), .1f, 1000.0f);
	m_UBO->Bind();
	m_UBO->SetData(0, sizeof(glm::mat4), glm::value_ptr(projection));

	m_Map[POINT] = "Point";
	m_Map[COORD] = "Frag Coord";
	m_Map[FRONT] = "Front Facing Texture";
	m_Map[CUBES] = "Color Cubes";

	m_CubeColors.push_back(RED);
	m_CubeColors.push_back(GREEN);
	m_CubeColors.push_back(BLUE);
	m_CubeColors.push_back(YELLOW);

	m_CubePositions.push_back(glm::vec3(-0.75f, 0.75f, 0.0f));
	m_CubePositions.push_back(glm::vec3(0.75f, 0.75f, 0.0f));
	m_CubePositions.push_back(glm::vec3(0.75f, -0.75f, 0.0f));
	m_CubePositions.push_back(glm::vec3(-0.75f, -0.75f, 0.0f));

	m_FrontFacingShader->Bind();
	m_FrontFacingShader->SetUniform1i("frontTexture", 0);
	m_FrontFacingShader->SetUniform1i("backTexture", 1);
	m_FrontFacingShader->Unbind();

}

AdvancedGLSL::~AdvancedGLSL() = default;

void AdvancedGLSL::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 view = m_Camera.GetViewMatrix();
	m_UBO->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

	glm::mat4 model = glm::mat4(1.0f);

	switch (m_DrawType)
	{
	case POINT:
		glEnable(GL_PROGRAM_POINT_SIZE);
		model = glm::translate(model, glm::vec3(0.0f));
		m_PointShader->Bind();
		m_PointShader->SetUniformMat4f("model", model);
		m_PointShader->SetUniform1i("pointSize", 64);
		m_Cube->DrawPoints(*m_PointShader, m_Context.Renderer);
		break;
	case COORD:
		glDisable(GL_PROGRAM_POINT_SIZE);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f));
		m_CoordShader->Bind();
		m_CoordShader->SetUniformMat4f("model", model);
		m_CoordShader->SetUniform1i("windowWidth", m_Camera.GetScreenWidth() / 2);
		m_Cube->Draw(*m_CoordShader, m_Context.Renderer);
		break;
	case FRONT:
		glDisable(GL_PROGRAM_POINT_SIZE);
		model = glm::mat4(1.0f);
		m_FrontFacingShader->SetUniformMat4f("model", model);
		m_FaceTexture->Bind(0);
		m_BackTexture->Bind(1);
		m_Cube->Draw(*m_FrontFacingShader, m_Context.Renderer);
		break;
	case CUBES:
		glDisable(GL_PROGRAM_POINT_SIZE);

		for (int i = 0; i < m_CubeColors.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, m_CubePositions[i]);
			m_ColorCubesShader->Bind();
			m_ColorCubesShader->SetUniformVec3("color", m_CubeColors[i]);
			m_ColorCubesShader->SetUniformMat4f("model", model);
			m_Cube->Draw(*m_ColorCubesShader, m_Context.Renderer);
		}
		
		break;
	}
}

void AdvancedGLSL::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0, 12.0), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Advanced GLSL", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		std::string selected = "Current selection: " + m_Map[m_DrawType];
		ImGui::Text(selected.c_str());
		if (ImGui::BeginCombo("Draw Type", m_Map[m_DrawType].c_str()))
		{
			for (auto choice : m_Map)
			{
				bool selected = (choice.second == m_Map[m_DrawType]);
				if (ImGui::Selectable(choice.second.c_str(), selected))
				{
					m_DrawType = choice.first;
				}
			}
			ImGui::EndCombo();
		}
		if (m_DrawType == FRONT)
		{
			ImGui::Separator();
			ImGui::Text("Whats in the box?");
		}
	}
	ImGui::End();
}
