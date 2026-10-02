#pragma once

#include "Core/AsyncLoader.h"
#include "Scene/Scene3D.h"

#include <memory>

class Shader;
class Model;

class ModelLoading : public Scene3D
{
public:
	ModelLoading(const SceneContext& context);
	~ModelLoading();

	void Render() override;
	void Update(float deltaTime) override;
	void OnGui() override;
private:
	std::unique_ptr<Shader> m_Shader;
	AsyncLoader<std::unique_ptr<Model>> m_Loader;
	std::vector<std::unique_ptr<Model>> m_Models;

	glm::vec3 m_LightPosition;

	std::string m_LastLoadError;
};
