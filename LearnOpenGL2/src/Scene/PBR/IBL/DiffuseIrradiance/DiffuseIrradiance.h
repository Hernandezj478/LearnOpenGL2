#pragma once
#include "Scene/Scene3D.h"

#include <memory>
#include <vector>

class Shader;
class Sphere;
class Texture;
class Cube;
class Skybox;
class FrameBuffer;
class RenderBuffer;
class Cubemap;
class LightMarker;

class DiffuseIrradiance : public Scene3D
{
public:
	DiffuseIrradiance(const SceneContext& context);
	~DiffuseIrradiance() = default;

	void Render() override;
private:
	std::unique_ptr<Sphere> m_Sphere;
	std::unique_ptr<Skybox> m_Skybox;
	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<LightMarker> m_Light;

	std::unique_ptr<Shader> m_PBRDiffuse;
	std::unique_ptr<Shader> m_EnvShader;
	std::unique_ptr<Shader> m_EquiRectMap;
	std::unique_ptr<Shader> m_IrradianceMap;

	std::unique_ptr<Texture> m_LoftRoom;

	std::unique_ptr<FrameBuffer> m_captureFBO;
	std::unique_ptr<RenderBuffer> m_captureRBO;
	std::unique_ptr<Cubemap> m_envCubemap;
	std::unique_ptr<Cubemap> m_IrradianceCubemap;

	std::vector<std::pair<glm::vec3, glm::vec3>> m_LightData;
	const int m_Rows = 7;
	const int m_Cols = 7;
	const float m_Spacing = 2.5f;
	bool m_UseGamma = true;
};