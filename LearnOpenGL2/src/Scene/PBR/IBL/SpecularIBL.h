#pragma once

#include "../Scene.h"

class SpecularIBL : public Scene
{
public:
	SpecularIBL(int width, int height);
	virtual void Run(GLFWwindow* window) override;
};