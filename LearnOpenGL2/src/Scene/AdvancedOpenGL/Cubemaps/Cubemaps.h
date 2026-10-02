#pragma once

#include "Core/AsyncLoader.h"
#include "Scene/Scene3D.h"

#include <memory>
#include <vector>

class Cube;
class Cubemap;
class Grid;
class Model;
class Plane;
class Shader;
class Skybox;
class Texture;

class Cubemaps : public Scene3D
{
public:
	Cubemaps(const SceneContext& context);
	~Cubemaps();

	void Render() override;
	void Update(float deltaTime) override;
	void OnGui() override;
private:
	AsyncLoader<std::unique_ptr<Model>> m_Loader;
	std::vector<std::unique_ptr<Model>> m_Models;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Cubemap> m_Cubemap;
	std::unique_ptr<Skybox> m_Skybox;

	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_SkyboxShader;
	std::unique_ptr<Shader> m_GridShader;
	std::unique_ptr<Shader> m_ReflectionShader;
	std::unique_ptr<Shader> m_RefractionShader;

	std::unique_ptr<Texture> m_CubeTexture;
	std::unique_ptr<Texture> m_PlaneTexture;
	std::unique_ptr<Texture> m_GroundTexture;

	std::unique_ptr<Plane> m_Plane;
	std::unique_ptr<Grid> m_Grid;

	std::vector<std::string> m_SkyboxFilepaths;
	std::vector<glm::vec3> m_CubePositions;

	bool bWireframeMode = false;
	float m_GroudScale = 10.0f;
	std::string m_LastLoadError;
};