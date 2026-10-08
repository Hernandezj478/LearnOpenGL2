#pragma once
#include "Scene/Scene3D.h"
#include "Core/AsyncLoader.h"

#include <memory>
#include <vector>
#include <utility>
#include <map>

class FrameBuffer;
class Shader;
class Texture;
class RenderTexture;
class RenderBuffer;
class Model;
class Plane2D;
class LightMarker;
class Sphere;

class DeferredShading : public Scene3D
{
public:
	DeferredShading(const SceneContext& context);
	~DeferredShading() = default;
	void Render() override;
	void OnGui() override;

private:

	enum Selection
	{
		TEST,
		NORMAL
	};

	std::unique_ptr<RenderTexture> m_Position;
	std::unique_ptr<RenderTexture> m_Normal;
	std::unique_ptr<RenderTexture> m_AlbedoSpec;

	std::unique_ptr<FrameBuffer> m_GBuffer;
	std::unique_ptr<RenderBuffer> m_rboDepth;

	std::unique_ptr<Shader> m_GeometryPass;
	std::unique_ptr<Shader> m_LightingPass;
	std::unique_ptr<Shader> m_GBufferShaderTest;

	std::unique_ptr<Plane2D> m_ScreenQuad;
	std::unique_ptr<Model> m_Backpack;
	std::unique_ptr<Sphere> m_LightVolume;

	std::unique_ptr<LightMarker> m_Light;

	std::vector<glm::vec3> m_ObjectPositions;
	std::vector<std::pair<glm::vec3, glm::vec3>> m_LightData;

	std::map<Selection, std::string> m_SceneSelection;
	Selection m_CurrentSelection;

	void DrawDeferredTest();
	void DrawDeferredLighting();
	void DrawLights();
};