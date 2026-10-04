#pragma once
#include "Scene/Scene3D.h"

#include <memory>
#include <vector>


class Plane2D;
class Cube;
class Shader;
class Texture;
class FrameBuffer;
class RenderBuffer;
class RenderTexture;
class LightMarker;

class HDR : public Scene3D
{
public:
	HDR(const SceneContext& context);
	~HDR() = default;

	void Render() override;
	void OnGui() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_hdrShader;
	std::unique_ptr<Texture> m_WoodTexture;

	std::unique_ptr<Plane2D> m_ScreenQuad;
	std::unique_ptr<Cube> m_Cube;
	
	std::unique_ptr<RenderBuffer> m_rboDepth;
	std::unique_ptr<FrameBuffer> m_hdrFBO;
	std::unique_ptr<RenderTexture> m_Colorbuffer;
	
	std::unique_ptr<LightMarker> m_Light;

	std::vector<glm::vec3> m_LightPositions;
	std::vector<glm::vec3> m_LightColors;

	float m_Exposure = 1.0;
};