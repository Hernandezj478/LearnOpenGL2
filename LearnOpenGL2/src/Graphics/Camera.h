#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraZoom
{
	IN,
	OUT,
	RESET
};

enum MovementDirection
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	Camera(	glm::vec3 pos	= glm::vec3(0.0f, 1.0f,  3.0f),
			glm::vec3 front	= glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3 up	= glm::vec3(0.0f, 1.0f,  0.0f),
			float pitch	= 0.0f, float yaw = -90.0f);
	void CameraMovement(MovementDirection direction, float deltaTime);
	void PocessMouseMovement(double xoffset, double yoffset);
	void ProcessMouseScroll(double yoffset);
	void AdjustFOV(CameraZoom zoom);

	void SetScreenSize(int width, int height);
	void SetAspectRatio();

	void ToggleFPS();

	inline glm::mat4 GetViewMatrix() { return glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp); }
	inline float GetFOV() const { return FOV; }
	inline float GetAspectRatio() const { return AspectRatio; }
	inline glm::vec3 GetPosition() const { return CameraPos; }
	inline glm::vec3 GetFront() const { return CameraFront; }
	inline int GetScreenWidth() const { return ScreenWidth; }
	inline int GetScreenHeight() const { return ScreenHeight; }

private:
	glm::vec3 WorldUp;

	glm::vec3 CameraPos;
	glm::vec3 CameraFront;
	glm::vec3 CameraRight;
	glm::vec3 CameraUp;

	int ScreenWidth;
	int ScreenHeight;

	float Pitch;
	float Yaw;

	bool rightMouseHold = false;

	float AspectRatio;
	float FOV = 45.0f;
	float cameraSpeedMultiplier = 2.5f;

	bool isFPSCamera = false;
	float FPSCamAlt = 1.8f;

	void UpdateCameraVectors();
};