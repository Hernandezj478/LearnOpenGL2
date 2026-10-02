#pragma once

#include "../Scene.h"

class DeferredShading : public Scene
{
private:

public:
	DeferredShading(int width, int height);
	void Run(GLFWwindow* window) override;
	void AdjustScreenSize(int width, int height) override;
};