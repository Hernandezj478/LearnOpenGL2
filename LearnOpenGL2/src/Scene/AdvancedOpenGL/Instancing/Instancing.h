#pragma once
#include "Scene/Scene3D.h"
#include "Core/AsyncLoader.h"

#include <memory>
#include <vector>
#include <map>
#include <string>

class Shader;
class Model;
class Sphere;
class Plane;
class Texture;

class Instancing : public Scene3D
{
public:
	Instancing(const SceneContext& context);
	~Instancing();

	void Render() override;
	void Update(float deltaTime) override;
	void OnGui() override;
private:

	enum SceneSelection
	{
		SIMPLE,
		SPACE
	};

	struct LoadedModel
	{
		std::unique_ptr<Model> model;
		std::vector<glm::mat4> instanceMatrices;
	};
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_SkyboxShader;
	std::unique_ptr<Shader> m_PlaneShader;

	std::unique_ptr<Sphere> m_Skybox;
	std::unique_ptr<Plane> m_Plane;

	std::unique_ptr<Texture> m_SkyboxTexture;

	AsyncLoader<LoadedModel> m_Loader;
	std::vector<std::unique_ptr<Model>> m_Models;
	std::string m_LastLoadError;

	std::map<SceneSelection, std::string> m_SceneMap;
	SceneSelection m_CurrentSelection = SIMPLE;
};