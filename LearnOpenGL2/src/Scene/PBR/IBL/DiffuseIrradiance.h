#pragma once

#include "../Scene.h"

class DiffuseIrradiance : public Scene
{
public:
	DiffuseIrradiance(int width, int height);
	virtual void Run(GLFWwindow* window) override;
};