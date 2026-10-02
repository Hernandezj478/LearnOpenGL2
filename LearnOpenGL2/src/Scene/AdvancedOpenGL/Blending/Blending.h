#pragma once
#include "Scene/Scene3D.h"
#include <memory>
#include <vector>

class Shader;
class Texture;
class Cube;
class Plane;
class Grid;

class Blending : public Scene3D
{
public:
	Blending(const SceneContext& context);
	~Blending();

	void Render() override;
	void OnGui() override;

private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_GridShader;
	std::unique_ptr<Texture> m_CubeTexture;
	std::unique_ptr<Texture> m_PlaneTexture;
	std::unique_ptr<Texture> m_GrassTexture;
	std::unique_ptr<Texture> m_GroundTexture;
	std::unique_ptr<Texture> m_WindowTexture;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane> m_Plane;
	std::unique_ptr<Grid> m_Grid;

	std::vector<glm::vec3> m_CubePositions;
	std::vector<glm::vec3> m_GrassPositions;
	std::vector<glm::vec3> m_WindowPositions;

	int m_GrassCount = 50;
	int m_WindowCount = 4;
	int m_GroundSize = 10;
	bool bGrassCountUpdate = true;
	bool bWindowCountUpdate = true;
};