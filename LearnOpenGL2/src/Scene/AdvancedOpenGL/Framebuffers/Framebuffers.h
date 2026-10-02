#pragma once

#include "Scene/Scene3D.h"
#include <Graphics/PostprocessFilters/PostprocessFilters.h>

#include <memory>
#include <vector>
#include <map>
#include <string>

class FrameBuffer;
class RenderBuffer;
class Shader;
class Texture;
class RenderTexture;
class Cube;
class Plane;
class Plane2D;
class Grid;

class Framebuffers : public Scene3D
{
public:
	Framebuffers(const SceneContext& context);
	~Framebuffers();

	void Render() override;
	void OnGui() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_GridShader;
	std::unique_ptr<Shader> m_KernelShader;

	std::shared_ptr<Shader> m_FrameShader;
	std::shared_ptr<Shader> m_InvertedShader;
	std::shared_ptr<Shader> m_GreyscaleShader;
	std::shared_ptr<Shader> m_SelectedShader;

	std::unique_ptr<Texture> m_CubeTexture;
	std::unique_ptr<Texture> m_PlaneTexture;
	std::unique_ptr<Texture> m_GrassTexture;
	std::unique_ptr<Texture> m_GroundTexture;

	std::unique_ptr<FrameBuffer> m_Framebuffer;
	std::unique_ptr<RenderTexture> m_Colorbuffer;
	std::unique_ptr<RenderBuffer> m_Renderbuffer;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane> m_Plane;
	std::unique_ptr<Plane2D> m_ScreenPlane;
	std::unique_ptr<Plane2D> m_MirrorPlane;
	std::unique_ptr<Grid> m_Grid;

	std::unique_ptr<PostProcessFilters> m_Filter;

	std::vector<glm::vec3> m_CubePositions;
	std::vector<glm::vec3> m_GrassPositions;
	int m_GrassToRender = 50;
	int m_GroundScale = 10;
	float m_BlurStrength;
	float m_TextureOffset;

	KernelType m_KernelSelection;
	int m_PostprocessSelection;

	std::map<KernelType, std::string> KernelMap;
	std::vector<std::string> m_PostprocessMap;

	bool m_WireframeMode = false;
	bool bFilter = false;

	void ChangeFilter();
	void ChangePostProcess();
};