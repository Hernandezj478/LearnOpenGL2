#pragma once
#include "Scene/Scene3D.h"
#include "Core/AsyncLoader.h"
#include <memory>
#include <vector>
#include <map>

class Shader;
class Sphere;
class Texture;
class Cube;
class Skybox;
class FrameBuffer;
class RenderBuffer;
class RenderTexture;
class Cubemap;
class LightMarker;
class Plane2D;
class Model;

using TextureMap = std::map<std::string, std::shared_ptr<Texture>>;

class SpecularIBL : public Scene3D
{
public:
	SpecularIBL(const SceneContext& context);
	~SpecularIBL() = default;

	void Render() override;
	void Update(float deltaTime) override;
	void OnGui() override;
private:
	enum SceneType
	{
		BASIC,
		TEXTURED,
		MODEL
	};

	std::unique_ptr<Sphere> m_Sphere;
	std::unique_ptr<Skybox> m_Skybox;
	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane2D> m_ScreenQuad;
	std::unique_ptr<LightMarker> m_Light;
	std::unique_ptr<Model> m_Backpack;

	std::unique_ptr<Shader> m_PBRDiffuse;
	std::unique_ptr<Shader> m_PBRTextured;
	std::unique_ptr<Shader> m_PBRModel;
	std::unique_ptr<Shader> m_EnvShader;
	std::unique_ptr<Shader> m_EquiRectMap;
	std::unique_ptr<Shader> m_IrradianceMap;
	std::unique_ptr<Shader> m_PrefilterMap;
	std::unique_ptr<Shader> m_BRDFShader;


	std::unique_ptr<Texture> m_LoftRoom;
	std::vector<TextureMap> m_SphereTextures;
	std::unique_ptr<RenderTexture> m_brdfLUT;

	std::unique_ptr<FrameBuffer> m_captureFBO;
	std::unique_ptr<RenderBuffer> m_captureRBO;
	std::unique_ptr<Cubemap> m_envCubemap;
	std::unique_ptr<Cubemap> m_IrradianceCubemap;
	std::unique_ptr<Cubemap> m_PrefilterCubemap;

	AsyncLoader<std::unique_ptr<Model>> m_ModelLoader;
	AsyncLoader<TextureMap> m_TextureLoader;

	std::vector<std::pair<glm::vec3, glm::vec3>> m_LightData;
	std::map<SceneType, std::string> m_SceneMap;
	SceneType m_SelectedScene = BASIC;
	const int m_Rows = 7;
	const int m_Cols = 7;
	const float m_Spacing = 2.5f;
	bool m_UseGamma = true;

	std::string m_LastError = "";

	void InitializeSphereTexutres();
	void RenderBasicScene();
	void RenderTexturedScene();
	void RenderModelScene();
};