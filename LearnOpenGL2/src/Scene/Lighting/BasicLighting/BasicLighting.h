#pragma once

#include "Scene/Scene3D.h"
#include "Graphics/LightMarker.h"

#include <memory>
#include <vector>

class Shader;
class Cube;

class BasicLighting : public Scene3D
{
public:
	explicit BasicLighting(const SceneContext& context);
	~BasicLighting();

	void Render() override;
	void OnGui() override;

private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Cube> m_Cube;

	LightMarker m_LightMarker;
	glm::vec3 lightPosition;

	float m_AmbientStrength = 0.1f;
	float m_SpecularStrength = 0.5f;
	int m_Shininess = 32;
	bool m_LightsDirty = true;

	std::vector<int> m_ShininessValues;

	void SetShininessValues();
	void UpdateLighting();
};
