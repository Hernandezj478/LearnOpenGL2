#pragma once
#include "Camera.h"
#include <stdexcept>

class Application
{
protected:
	Camera camera;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

public:
	Application()
	{
		throw std::runtime_error("Class is deprecated. Use updated Application class Scene/Scene3D")
	}
	virtual void Run(GLFWwindow* window) = 0;
	virtual void AdjustScreenSize(int width, int height)
	{
		camera.SetScreenSize(width, height);
		camera.SetAspectRatio();
	}

	virtual void ProcessInput(GLFWwindow* window, int key, int action)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
		{
			switch (action)
			{
			case GLFW_PRESS:
			{
				glfwSetWindowShouldClose(window, true);
				return;
			}
			default:
				// No action
				return;
			}
		}
		case GLFW_KEY_P:
		{
			switch (action)
			{
			case GLFW_PRESS:
			{
				camera.ToggleFPS();
				return;
			}
			default: return;
			}
		}
		default: return;//No action
		}
	}

	virtual void ProcessMovement(GLFWwindow* window)
	{
		//Camera Movement
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.CameraMovement(FORWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.CameraMovement(BACKWARD, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.CameraMovement(LEFT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.CameraMovement(RIGHT, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			camera.CameraMovement(UP, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			camera.CameraMovement(DOWN, deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		{
			camera.AdjustFOV(IN);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
		{
			camera.AdjustFOV(OUT);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS)
		{
			camera.AdjustFOV(RESET);
		}
	}

	virtual void Mouse_Callback(GLFWwindow* window, double xpos, double ypos)
	{
		camera.PocessMouseMovement(window, xpos, ypos);
	}
	virtual void Mouse_Button_Callback(GLFWwindow* window, int button, int action, int mods)
	{
		camera.ProcesMouseButton(window, button, action, mods);
	}
	virtual void Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(window, xoffset, yoffset);
	}
};