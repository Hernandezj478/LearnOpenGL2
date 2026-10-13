#pragma once
#include "Scene/Scene3D.h"

#include <memory>
#include <vector>

class FrameBuffer;
class Shader;
class Texture;
class RenderTexture;
class RenderBuffer;
class Model;
class Plane2D;
class Cube;
class LightMarker;

class SSAO : public Scene3D
{
public:
	SSAO(const SceneContext& context);
	~SSAO() = default;

	void Render() override;
	void OnGui() override;

	float lerp(float a, float b, float f);
private:
	std::unique_ptr<RenderTexture> m_Position;
	std::unique_ptr<RenderTexture> m_Normal;
	std::unique_ptr<RenderTexture> m_AlbedoSpec;
	std::unique_ptr<RenderTexture> m_NoiseTexture;
	std::unique_ptr<RenderTexture> m_ssaoColorBuffer;
	std::unique_ptr<RenderTexture> m_BlurBuffer;

	std::unique_ptr<Texture> m_RoomTexture;

	std::unique_ptr<FrameBuffer> m_GBuffer;
	std::unique_ptr<FrameBuffer> m_ssaoFBO;
	std::unique_ptr<FrameBuffer> m_BlurFBO;
	std::unique_ptr<RenderBuffer> m_rboDepth;

	std::unique_ptr<Shader> m_GeometryPass;
	std::unique_ptr<Shader> m_LightingPass;
	std::unique_ptr<Shader> m_ssaoPass;
	std::unique_ptr<Shader> m_BlurPass;

	std::unique_ptr<Shader> m_ssaoDebug;

	std::unique_ptr<Model> m_Backpack;
	std::unique_ptr<Cube> m_Room;
	std::unique_ptr<Plane2D> m_ScreenQuad;

	std::unique_ptr<LightMarker> m_Light;

	std::vector<glm::vec3> m_ssaoKernel;
	std::vector<glm::vec3> m_ssaoNoise;

	glm::vec3 m_LightPosition;
	glm::vec3 m_LightColor;
	bool m_UseSSAO = true;
};