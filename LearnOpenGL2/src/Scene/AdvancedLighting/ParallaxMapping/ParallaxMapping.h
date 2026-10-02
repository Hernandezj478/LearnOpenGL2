#pragma once

#include "../Scene.h"

class ParallaxMapping : public Scene
{
public:
	ParallaxMapping(int width, int height);
	void Run(GLFWwindow* window) override;
	void ProcessInput(GLFWwindow* window, int key, int action) override;
private:
	bool bNormal = true;
	bool bParallax = true;
};