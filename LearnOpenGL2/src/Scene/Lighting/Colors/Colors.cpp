#include "Colors.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Graphics/Renderer.h"
#include "Graphics/Shader.h"
#include "Graphics/ColorPalette.h"
#include "Graphics/Shapes/Cube.h"

#include "Scene/SceneContext.h"

Colors::Colors(const SceneContext& context) : Scene3D(context)
{
	m_Shader = std::make_unique<Shader>("res/shaders/Lighting/Color.glsl");
	m_Cube = std::make_unique<Cube>();
	glEnable(GL_DEPTH_TEST);
}

Colors::~Colors() = default;

void Colors::Render()
{
	m_Context.Renderer.Clear(BLACK, COLOR_DEPTH);

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 view = m_Camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera.GetFOV()), m_Camera.GetAspectRatio(), 0.1f, 1000.f);

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("projection", projection);
	m_Shader->SetUniformMat4f("view", view);
	m_Shader->SetUniformMat4f("model", model);
	m_Shader->SetUniformVec3("objectColor", ORANGE);
	m_Shader->SetUniformVec3("lightColor", WHITE);
	m_Cube->Draw(*m_Shader, m_Context.Renderer);

	glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);
	m_LightMarker.Draw(m_Context.Renderer, view, projection, lightPosition, WHITE);
}
