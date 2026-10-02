#pragma once

#include "../Scene.h"

class ShadowMapping : public Scene
{
public:
	ShadowMapping(int width, int height);
	void Run(GLFWwindow* window) override;
};