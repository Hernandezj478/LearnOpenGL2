#pragma once
#include "../Scene3D.h"

#include <memory>

class Shader;
class Texture;
class Cube;
class Plane;
class Grid;
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class VertexBufferLayout;

class Startup : public Scene3D
{
public:
	Startup(const SceneContext& context);
	~Startup();
	void Render() override;
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_GridShader;
	std::unique_ptr<Texture> m_Texture1;
	std::unique_ptr<Texture> m_Texture2;
	std::unique_ptr<Texture> m_Texture3;
	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Plane> m_Plane;
	std::unique_ptr<Grid> m_Grid;

	glm::vec3 cubePositions[10];
	glm::vec3* cubeRotations;
	const int frequency = 3;
};
