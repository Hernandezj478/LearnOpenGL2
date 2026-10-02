#pragma once


#include "../Scene.h"

class BlinnPhong : public Scene
{
private:
	bool isBlinn = true;
public:
	BlinnPhong(int width, int height);
	void Run(GLFWwindow* window) override;

	void ProcessInput(GLFWwindow* window, int key, int action) override;
};
