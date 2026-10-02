#include "LightMarker.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Graphics/Shader.h"
#include "Graphics/Shapes/Sphere.h"

LightMarker::LightMarker() : m_Shader(std::make_unique<Shader>("res/shaders/Lighting/LightSource.glsl")),
    m_Light(std::make_unique<Sphere>())
{
}

LightMarker::~LightMarker() = default;

void LightMarker::Draw(const Renderer & renderer, const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & position, const glm::vec3 & color, float brightness, float scale)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, glm::vec3(scale));
    m_Shader->Bind();
    m_Shader->SetUniformMat4f("projection", projection);
    m_Shader->SetUniformMat4f("view", view);
    m_Shader->SetUniformMat4f("model", model);
    m_Shader->SetUniformVec3("lightColor", color);
    m_Shader->SetUniform1f("brightness", brightness);
    m_Light->Draw(*m_Shader, renderer);
    m_Shader->Unbind();
}
