#pragma once

#include "Scene/Scene3D.h"
#include "Graphics/LightMarker.h"
#include "Graphics/MaterialLibrary.h"

#include <memory>
#include <string>

class Shader;
class Cube;

class Materials : public Scene3D
{
public:
	explicit Materials(const SceneContext& context);
	~Materials();

	void Render() override;
	void OnGui() override;

private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Cube> m_Cube;

	LightMarker m_LightMarker;
	Mat m_Material;
	std::string m_MaterialSelection;
	glm::vec3 m_LightPosition;

	bool bUseMaterial = false;
	bool bUsePartyLights = false;
	glm::vec3 lightColor;
	glm::vec3 m_LightAmbient;
	glm::vec3 m_LightDiffuse;
	glm::vec3 m_LightSpecular;
};