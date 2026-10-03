#pragma once
#include "Scene/Scene3D.h"

#include <memory>
#include <vector>

class Texture;
class Shader;
class Plane;
class LightMarker;

class ParallaxMapping : public Scene3D
{
public:
	ParallaxMapping(const SceneContext& context);
	~ParallaxMapping() = default;

	void Render() override;
	void OnGui() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Texture> m_WallAlbedo;
	std::unique_ptr<Texture> m_WallNormal;
	std::unique_ptr<Texture> m_WallHeight;

	std::unique_ptr<Texture> m_ToyAlbedo;
	std::unique_ptr<Texture> m_ToyNormal;
	std::unique_ptr<Texture> m_ToyHeight;

	std::unique_ptr<Plane> m_Plane;

	std::unique_ptr<LightMarker> m_Light;

	glm::vec3 m_LightPosition;
	float m_HeightScale = 0.1f;
};