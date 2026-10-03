#pragma once
#include "Scene/Scene3D.h"


#include <memory>

class LightMarker;
class Plane;
class Texture;
class Shader;


class NormalMapping : public Scene3D
{
public:
	NormalMapping(const SceneContext& context);
	~NormalMapping() = default;

	void Render() override;
private:
	std::unique_ptr<Texture> m_WallAlbedo;
	std::unique_ptr<Texture> m_WallNormal;

	std::unique_ptr<Shader> m_Shader;

	std::unique_ptr<Plane> m_Wall;

	std::unique_ptr<LightMarker> m_Light;
	glm::vec3 m_LightPosition;

};