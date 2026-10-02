#pragma once

#include "../Scene.h"

class Bloom : public Scene
{
public:
	Bloom(int screenWidth, int screenHeight);
	void Run(GLFWwindow* window) override;
	void ProcessInput(GLFWwindow* window, int key, int action) override;
	void AdjustScreenSize(int width, int height) override;

	void CheckFramebufferStatus(const char* name);
	void CheckStatus();
private:
	bool bloom = true;
	bool bloomKeyPressed = false;
	float exposure = 1.0f;
};
