#pragma once

#include "../Scene.h"

class SSAO : public Scene
{
public:
	SSAO(int width, int height);
	virtual void Run(GLFWwindow* window) override;
private:
	float ourLerp(float a, float b, float f);
};