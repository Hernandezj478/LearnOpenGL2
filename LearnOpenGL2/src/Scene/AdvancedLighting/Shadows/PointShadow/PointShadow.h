#pragma once

#include "Scene/Scene3D.h"

#include <memory>
#include <vector>

class Cube;
class Cubemap;
class FrameBuffer;
class LightMarker;
class Shader;
class Texture;


class PointShadow : public Scene3D
{
public:
	PointShadow(const SceneContext& context);
	~PointShadow() = default;

	void Render() override;

private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_DepthShader;
	std::unique_ptr<Texture> m_WoodTexture;
	std::unique_ptr<Cubemap> m_DepthCubeMap;
	std::unique_ptr<FrameBuffer> m_DepthMapFBO;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<LightMarker> m_Light;

	std::vector<glm::vec3> m_ShadowTransformTarget;
	std::vector<glm::vec3> m_ShadowUpVectors;
	std::vector<glm::vec3> m_CubePositions;
	std::vector<glm::vec3> m_CubeScales;

	static const int SHADOW_WIDTH = 1024;
	static const int SHADOW_HEIGHT = 1024;

	glm::vec3 m_LightPos = glm::vec3(0.0f, 0.0f, 0.0f);
	bool m_DebugCubeVertices = true;


	void RenderScene(Shader& Shader);
};