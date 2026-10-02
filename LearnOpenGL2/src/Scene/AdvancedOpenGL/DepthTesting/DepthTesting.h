#pragma once

#include "Scene/Scene3D.h"

#include <memory>
#include <vector>

class Cube;
class Plane;
class Grid;
class Texture;
class Shader;

class DepthTesting : public Scene3D
{
public:
	DepthTesting(const SceneContext& context);
	~DepthTesting();

	void Render() override;
	void OnGui() override;

private:
	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane> m_Plane;
	std::unique_ptr<Grid> m_Grid;
	std::unique_ptr<Texture> m_CubeTexture;
	std::unique_ptr<Texture> m_PlaneTexture;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_GridShader;

	std::vector<glm::vec3> m_CubePositions;

	bool m_DepthTest = false;
	float m_ZNear = 0.1f;
	float m_ZFar = 10.0f;
};