#pragma once
#include "Scene/Scene3D.h"

#include <memory>
#include <vector>

class Plane;
class Shader;
class Texture;

class GammaCorrection : public Scene3D
{
public:
	GammaCorrection(const SceneContext& context);
	~GammaCorrection() = default;

	void Render() override;
	void OnGui() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Texture> m_FloorTexture;
	std::unique_ptr<Texture> m_FloorTextureGamma;
	std::unique_ptr<Plane> m_Plane;

	std::vector<glm::vec3> m_LightPositions;
	std::vector<glm::vec3> m_LightColors;

	bool m_UseGamma = false;
	bool m_UseBlinnPhong = false;
	bool m_UseLinearGamma = false;
	float m_UVScale = 3.0f;
};