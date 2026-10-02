#pragma once

#include "../Scene.h"

class PBR : public Scene
{
public:
	PBR(int width, int height);
	virtual void Run(GLFWwindow* window) override;
};