#pragma once

#include "Scene/Scene3D.h"
#include "Graphics/LightMarker.h"

#include <memory>
#include <vector>

class Shader;
class Texture;
class Cube;

class LightCasters : public Scene3D
{
public:
	explicit LightCasters(const SceneContext& context);
	~LightCasters();

	void Render() override;
	void OnGui() override;
private:
	enum LightTypes
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};
	std::shared_ptr<Shader> m_ShaderPtr;
	std::shared_ptr<Shader> m_DirectionalLight;
	std::shared_ptr<Shader> m_PointLight;
	std::shared_ptr<Shader> m_SpotLight;
	std::unique_ptr<Texture> m_DiffuseTexture;
	std::unique_ptr<Texture> m_SpecularTexture;
	std::unique_ptr<Cube> m_Cube;

	LightMarker m_LightMarker;
	LightTypes m_LightSelection = DIRECTIONAL;

	glm::vec3 m_LightDirection;
	glm::vec3 m_PointlightPosition;
	glm::vec3 m_SpotlightPosition;

	std::vector<glm::vec3> m_CubePositions;

	bool bLightDirty = true;

	void LoadDirectionalLights();
	void LoadPointlights();
	void LoadSpotlights();

	void UpdateLighting();
};