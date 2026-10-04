#pragma once

#include "../Scene.h"

class HDR : public Scene
{
public:
	HDR(int width, int height);
	void Run(GLFWwindow* window);

	void ProcessInput(GLFWwindow* window, int key, int action) override;
	void AdjustScreenSize(int width, int height) override;
private:
	bool hdr = true;
	bool hdrKeyPressed = false;
	float exposure = 1.0f;
};