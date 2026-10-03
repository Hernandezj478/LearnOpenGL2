#pragma once

#include "Scene/Scene3D.h"

#include <memory>
#include <vector>
#include <map>

class Cube;
class FrameBuffer;
class LightMarker;
class Plane;
class Plane2D;
class RenderTexture;
class Shader;
class Texture;

class ShadowMapping : public Scene3D
{
public:
	ShadowMapping(const SceneContext& context);
	~ShadowMapping() = default;

	void Render() override;
	void OnGui() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_DepthShader;
	std::unique_ptr<Shader> m_DepthDebug;

	std::unique_ptr<Texture> m_FloorTexture;
	std::unique_ptr<RenderTexture> m_DepthBuffer;
	std::unique_ptr<FrameBuffer> m_DepthmapFBO;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane> m_Floor;
	std::unique_ptr<Plane2D> m_ScreenQuad;

	std::unique_ptr<LightMarker> m_Light;

	const int SHADOW_WIDTH = 1024;
	const int SHADOW_HEIGHT = 1024;

	std::vector<glm::vec3> m_CubePositions;
	std::vector<glm::vec3> m_CubeScales;
	glm::vec3 m_LightPosition;

	bool m_DebugMode = false;
	bool m_UseOrtho = true;
};