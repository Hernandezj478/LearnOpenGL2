#pragma once
#include "Scene/Scene3d.h"
#include <memory>
#include <vector>

class Shader;
class Texture;
class Cube;
class Plane;
class Grid;

class StencilTesting : public Scene3D
{
public:
	StencilTesting(const SceneContext& context);
	~StencilTesting();

	void Render() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_GridShader;
	std::unique_ptr<Shader> m_SingleColor;
	std::unique_ptr<Texture> m_CubeTexture;
	std::unique_ptr<Texture> m_PlaneTexture;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane> m_Plane;
	std::unique_ptr<Grid> m_Grid;

	std::vector<glm::vec3> m_CubePositions;
};