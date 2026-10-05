#pragma once
#include "Scene/Scene3D.h"

#include <memory>
#include <vector>
#include <utility>

class Shader;
class Texture;
class Cube;
class Plane;
class Plane2D;
class FrameBuffer;
class RenderBuffer;
class RenderTexture;
class LightMarker;

class Bloom : public Scene3D
{
public:
	Bloom(const SceneContext& context);
	~Bloom() = default;

	void Render() override;
	void OnGui() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_Blur;
	std::unique_ptr<Shader> m_Bloom;

	std::unique_ptr<Texture> m_WoodTexture;
	std::unique_ptr<Texture> m_ContainerTexture;

	std::unique_ptr<FrameBuffer> m_hdrFBO;
	std::unique_ptr<FrameBuffer> m_PingPongFBO;
	std::unique_ptr<RenderTexture> m_Colorbuffers;
	std::unique_ptr<RenderTexture> m_PingPongColorbuffers;
	std::unique_ptr<RenderBuffer> m_rboDepth;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane> m_Ground;
	std::unique_ptr<Plane2D> m_ScreenQuad;

	std::unique_ptr<LightMarker> m_Light;

	std::vector<std::pair<glm::vec3, glm::vec3>> m_LightData;
	std::vector<std::pair<glm::vec3, glm::vec3>> m_CubeData;

	bool m_UseBloom = true;
	float m_Exposure = 1.0f;
};