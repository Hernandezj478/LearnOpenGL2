#pragma once
#include "Scene/Scene3D.h"
#include "Core/AsyncLoader.h"

#include <memory>
#include <vector>
#include <utility>
#include <map>

class Shader;
class Sphere;
class Texture;
class LightMarker;
class Model;

class PBRLighting : public Scene3D
{
public:
	PBRLighting(const SceneContext& context);
	~PBRLighting() = default;

	void Render() override;
	void Update(float deltaTime) override;
	void OnGui() override;
private:
	enum SceneType
	{
		BASIC,
		TEXTURE,
		MODEL
	};
	std::unique_ptr<Shader> m_PBRLighting;
	std::unique_ptr<Shader> m_PBRTexturedLighting;
	std::unique_ptr<Shader> m_PBRModelLighting;

	std::unique_ptr<Texture> m_Albedo;
	std::unique_ptr<Texture> m_Normal;
	std::unique_ptr<Texture> m_Metallic;
	std::unique_ptr<Texture> m_Roughness;
	std::unique_ptr<Texture> m_AO;

	std::unique_ptr<Model> m_Backpack;
	std::unique_ptr<Sphere> m_Sphere;
	std::unique_ptr<LightMarker> m_Light;

	std::vector<std::pair<glm::vec3, glm::vec3>> m_LightData;

	std::map<SceneType, std::string> m_SceneSelection;
	SceneType m_SelectedScene = TEXTURE;

	AsyncLoader<std::unique_ptr<Model>> m_ModelLoader;

	int m_Rows = 7; 
	int m_Cols = 7;
	float m_Spacing = 2.5f;
	bool m_UseGamma = true;
	std::string m_LastError = "";

	void BasicPBR();
	void TexturedPBR();
	void ModelPBR();
};