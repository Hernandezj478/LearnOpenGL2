#pragma once

#include "Scene/Scene3D.h"
#include "Graphics/LightMarker.h"

#include <memory>

class Shader;
class Texture;
class Cube;

class LightingMaps : public Scene3D
{
public:
	LightingMaps(const SceneContext& context);
	~LightingMaps();

	void Render() override;
private:
	std::unique_ptr<Shader> m_CubeShader;
	std::unique_ptr<Texture> m_DiffuseTexture;
	std::unique_ptr<Texture> m_SpecularTexture;
	std::unique_ptr<Cube> m_Cube;

	LightMarker m_LightMarker;
	glm::vec3 m_LightPos;
};