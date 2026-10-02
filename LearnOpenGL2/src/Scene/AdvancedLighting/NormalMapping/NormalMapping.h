#pragma once

#include "../Scene.h"

class NormalMapping : public Scene
{
public:
	NormalMapping(int width, int height);
	void Run(GLFWwindow* window) override;
};