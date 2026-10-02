#pragma once

#include "Scene/Scene3D.h"

#include <map>
#include <memory>
#include <string>

class Plane;
class Shader;
class Texture;
class LightMarker;

class AdvancedLighting : public Scene3D
{
public:
	AdvancedLighting(const SceneContext& context);
	~AdvancedLighting() = default;

	void Render() override;
	void OnGui() override;
private:
	enum LightType
	{
		POINT,
		SPOT
	};

	std::unique_ptr<Plane> m_Floor;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Texture> m_FloorTexture;
	std::unique_ptr<LightMarker> m_Light;

	bool bBlinnPhong = false;
	glm::vec3 m_LightPosition;
	LightType m_LightSelection = POINT;
	std::map<LightType, std::string> m_LightTypeMap;
};