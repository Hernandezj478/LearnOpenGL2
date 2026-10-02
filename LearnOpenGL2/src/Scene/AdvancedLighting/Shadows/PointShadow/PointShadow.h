#pragma once
#include "../Scene.h"

class PointShadow : public Scene
{
private:
	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f); 
	bool debugCubeVertices = true;
public:
	PointShadow(int width, int height);
	void Run(GLFWwindow* window) override;

	void RenderScene(class Shader& shader, const class Renderer& renderer);
};