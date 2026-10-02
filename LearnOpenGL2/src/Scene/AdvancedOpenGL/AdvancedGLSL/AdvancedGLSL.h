#pragma once

#include "Scene/Scene3D.h"

#include <memory>
#include <map>
#include <vector>

class Cube;
class Shader;
class UniformBuffer;
class Texture;

class AdvancedGLSL : public Scene3D
{
public:
	AdvancedGLSL(const SceneContext& context);
	~AdvancedGLSL();

	void Render() override;
	void OnGui() override;
private:
	enum DrawType
	{
		POINT,
		COORD,
		FRONT,
		CUBES
	};

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Shader> m_PointShader;
	std::unique_ptr<Shader> m_CoordShader;
	std::unique_ptr<Shader> m_FrontFacingShader;
	std::unique_ptr<Shader> m_ColorCubesShader;

	std::unique_ptr<Texture> m_FaceTexture;
	std::unique_ptr<Texture> m_BackTexture;

	std::unique_ptr<UniformBuffer> m_UBO;
	DrawType m_DrawType = POINT;
	std::map<DrawType, std::string> m_Map;
	std::vector<glm::vec3> m_CubeColors;
	std::vector<glm::vec3> m_CubePositions;
};