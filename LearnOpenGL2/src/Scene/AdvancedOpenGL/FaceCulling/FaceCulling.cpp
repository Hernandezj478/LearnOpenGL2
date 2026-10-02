#include "FaceCulling.h"

#include "Graphics/ColorPalette.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Graphics/Shapes/Cube.h"

#include "Scene/SceneContext.h"

#include <imgui/imgui.h>

FaceCulling::FaceCulling(const SceneContext& context) : Scene3D(context)
{
	glEnable(GL_CULL_FACE);
	m_Cube = std::make_unique<Cube>();
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedOpenGL/FaceCulling/FaceCulling.glsl");
	m_Texture = std::make_unique<Texture>("res/textures/GreyboxTexture.png");
	m_Texture->SyncTexture();

	m_Shader->SetUniform1i("texture1", 0);

	m_CullMap[GL_FRONT]				= "Front";
	m_CullMap[GL_BACK]				= "Back";
	m_CullMap[GL_FRONT_AND_BACK]	= "Front and Back";
	m_WindMap[GL_CCW]				= "Counter Clock-wise";
	m_WindMap[GL_CW]				= "Clock-wise";
}

FaceCulling::~FaceCulling() = default;

void FaceCulling::Render()
{

	glCullFace(m_FaceCullSelection);
	glFrontFace(m_WindingSelection);
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model(1.0f);
	glm::mat4 view(m_Camera.GetViewMatrix());
	glm::mat4 projection = glm::perspective(m_Camera.GetFOV(), m_Camera.GetAspectRatio(), 0.1f, 1000.0f);

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniform1f("UVTiling", 10.0);

	model = glm::translate(model, glm::vec3(0.0f));
	m_Shader->SetUniformMat4f("model", model);

	m_Texture->Bind(0);
	m_Cube->Draw(*m_Shader, m_Context.Renderer);
}

void FaceCulling::OnGui()
{
	ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Face Culling Test", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		if(ImGui::BeginCombo("Face Culling Type", m_CullMap[m_FaceCullSelection].c_str()))
		{
			for (auto it : m_CullMap)
			{
				bool isSelected = (it.first == m_FaceCullSelection);
				if (ImGui::Selectable(it.second.c_str(), isSelected))
				{
					m_FaceCullSelection = it.first;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();
		if (ImGui::BeginCombo("Winding Type", m_WindMap[m_WindingSelection].c_str()))
		{
			for (auto it : m_WindMap)
			{
				bool isSelected = (it.first == m_WindingSelection);
				if (ImGui::Selectable(it.second.c_str(), isSelected))
				{
					m_WindingSelection = it.first;
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::End();
}
