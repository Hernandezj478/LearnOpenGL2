#include "Camera.h"
#include <algorithm>

#define EPSILON 1e-6f

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float pitch, float yaw)
{
	CameraPos = pos;
	CameraFront = front;
	WorldUp = up;
	Pitch = pitch;
	Yaw = yaw;
	UpdateCameraVectors();
}

void Camera::CameraMovement(MovementDirection direction, float deltaTime)
{
	float velocity = cameraSpeedMultiplier * deltaTime;
	switch(direction)
	{
	case FORWARD:
		CameraPos += velocity * CameraFront;
		if (isFPSCamera) 
		{
			CameraPos = glm::vec3(CameraPos.x, FPSCamAlt, CameraPos.z);
		}
		return;
	case BACKWARD:
		CameraPos -= velocity * CameraFront;
		if (isFPSCamera)
		{
			CameraPos = glm::vec3(CameraPos.x, FPSCamAlt, CameraPos.z);
		}
		return;
	case LEFT:
		CameraPos -= glm::normalize(glm::cross(CameraFront, CameraUp)) * velocity;
		if (isFPSCamera)
		{
			CameraPos = glm::vec3(CameraPos.x, FPSCamAlt, CameraPos.z);
		}
		return;
	case RIGHT:
		CameraPos += glm::normalize(glm::cross(CameraFront, CameraUp)) * velocity;
		if (isFPSCamera)
		{
			CameraPos = glm::vec3(CameraPos.x, FPSCamAlt, CameraPos.z);
		}
		return;
	case UP:
		if (!isFPSCamera)
		{
			CameraPos += velocity * CameraUp;
		}
		return;
	case DOWN:
		if (!isFPSCamera)
		{
			CameraPos -= velocity * CameraUp;
		}
		return;
	default:
		// does nothing
		return;
	}

}

void Camera::PocessMouseMovement(double xoffset, double yoffset)
{
	if (xoffset == 0.0f && yoffset == 0)
	{
		return;
	}
	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	Yaw += xoffset;
	Pitch += yoffset;
	Pitch = std::clamp(Pitch, -89.0f, 89.0f);
	
	Yaw = std::fmod(Yaw, 360.0);
	if (Yaw < 0.0f)
	{
		Yaw += 360.0f;
	}

	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(double yoffset)
{
	cameraSpeedMultiplier = std::clamp(cameraSpeedMultiplier + yoffset, 1.0, 1000.0);
}

void Camera::AdjustFOV(CameraZoom zoom)
{
	float deltaAngle = 0.5f;
	switch(zoom)
	{
	case IN:
		FOV -= deltaAngle;
		if (FOV < 1.0f)
		{
			FOV = 1.0f;
		}
		return;
	case OUT:
		FOV += deltaAngle;
		if (FOV > 100.0f)
		{
			FOV = 100.0f;
		}
		return;
	case RESET:
		FOV = 45.0f;
		return;
	default:
		// This should never be called.
		return;
	}
}

void Camera::SetScreenSize(int width, int height)
{
	ScreenWidth = width;
	ScreenHeight = height;
}

void Camera::SetAspectRatio()
{
	AspectRatio = (float)ScreenWidth / (float)ScreenHeight;
}

void Camera::ToggleFPS()
{
	isFPSCamera = !isFPSCamera;
	if (isFPSCamera)
	{
		CameraPos = glm::vec3(CameraPos.x, FPSCamAlt, CameraPos.z);
	}
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	direction.y = -sin(glm::radians(Pitch));
	direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	CameraFront = glm::normalize(direction);
	CameraRight = glm::normalize(glm::cross(CameraFront, WorldUp));
	CameraUp	= glm::normalize(glm::cross(CameraRight, CameraFront));
}