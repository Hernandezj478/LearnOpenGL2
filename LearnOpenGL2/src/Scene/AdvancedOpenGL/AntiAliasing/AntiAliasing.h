#pragma once
#include "Scene/Scene3D.h"

#include <memory>

class Cube;
class Plane2D;

class FrameBuffer;
class RenderTexture;
class RenderBuffer;
class Shader;
class Texture;

class AntiAliasing : public Scene3D
{
public:
	AntiAliasing(const SceneContext& context);
	~AntiAliasing() = default;
	void Render() override;
private:
	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane2D> m_ScreenPlane;

	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_ScreenShader;
	std::unique_ptr<RenderTexture> m_RenderTexture;
	std::unique_ptr<RenderTexture> m_ScreenTexture;
	std::unique_ptr<Texture> m_CubeTexture;

	std::unique_ptr<FrameBuffer> m_Framebuffer;
	std::unique_ptr<FrameBuffer> m_IFB;
	std::unique_ptr<RenderBuffer> m_Renderbuffer;
};