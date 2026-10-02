#pragma once

#include "Scene/Scene3D.h"
#include "Core/AsyncLoader.h"
#include <memory>
#include <vector>
#include <map>

class Model;
class Shader;
class Cube;
class Plane;
class UniformBuffer;

class GeometryShader : public Scene3D
{
public:
	GeometryShader(const SceneContext& context);
	~GeometryShader();

	void Render() override;
	void Update(float deltaTime) override;
	void OnGui() override;
private:
	enum Options
	{
		PRIMPOINT,
		PLANEPTS,
		HOUSEPTS,
		HARYMODEL,
		EXPLODE
	};
	AsyncLoader<std::unique_ptr<Model>> m_Loader;
	std::vector<std::unique_ptr<Model>> m_Models;

	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_NormalShader;
	std::unique_ptr<Shader> m_CubePointShader;
	std::unique_ptr<Shader> m_PlanePointShader;
	std::unique_ptr<Shader> m_PointHouseShader;
	std::unique_ptr<Shader> m_ExplosionShader;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane> m_Plane;
	std::unique_ptr<UniformBuffer> m_UBO;

	std::string m_LastLoadError;
	std::map<Options, std::string> m_OptionMap;

	Options m_SceneOption = PRIMPOINT;
};