#pragma once

#include "Scene/Scene3D.h"
#include "Graphics/LightMarker.h"

#include "glm/glm.hpp"

#include <memory>
#include <unordered_map>
#include <map>
#include <vector>

class Shader;
class Texture;
class Cube;

class MultipleLights : public Scene3D
{
public:
	explicit MultipleLights(const SceneContext& context);
	~MultipleLights();

	void Update(float deltaTime) override;
	void Render() override;
	void OnGui() override;

private:
	enum SceneType
	{
		DESERT = 0, FACTORY = 1, HORROR = 2, LAB = 3
	};
	void SetLightColors();
	void SetLightFalloffValues();
	void SetCubePositions(int numCubes, bool randomPos);
	void UpdateLighting();

	std::unordered_map<SceneType, glm::vec4> m_ClearColors;
	std::unordered_map<SceneType, glm::vec4> m_DirectionalColors;
	std::unordered_map<SceneType, std::vector<glm::vec4>> m_PointLightColors;
	std::unordered_map<SceneType, glm::vec4> m_SpotlightColors;

	std::map<int, glm::vec3> m_LightFalloffValues;

	std::unique_ptr<Shader> m_LightShader;
	std::unique_ptr<Texture> m_DiffuseMap;
	std::unique_ptr<Texture> m_SpecularMap;
	std::unique_ptr<Cube> m_Cube;

	LightMarker m_Marker;

	std::vector<glm::vec3> m_CubePositions;
	SceneType m_SceneSelect = DESERT;
	int m_LightRange = 50;
	bool m_DirectionalOn = true;
	bool m_PointLightsOn = true;
	bool bIsFlashlightOn = false;
	float m_Brightness = 1.0f;
	float m_AmbientStrength = 1.0f;
	float m_DiffuseStrength = 1.0f;
	float m_SpecularStrength = 1.0f;
	bool m_LightsDirty = true;
};