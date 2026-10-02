#include "Scene3D.h"

#include "Core/Input.h"
#include "Core/Window.h"
#include "SceneContext.h"
Scene3D::Scene3D(const SceneContext & context) : Scene(context)
{
	const glm::ivec2 size = context.Window.FramebufferSize();
	m_Camera.SetScreenSize(size.x, size.y);
	m_Camera.SetAspectRatio();
}

void Scene3D::Update(float deltaTime)
{
	DeltaTime = deltaTime;
	UpdateCamera();
}

void Scene3D::OnResize(int width, int height)
{
	m_Camera.SetScreenSize(width, height);
	m_Camera.SetAspectRatio();
}

void Scene3D::UpdateCamera()
{
	const Input& input = m_Context.Input;
	if (input.IsPressed(GLFW_KEY_P))
	{
		m_Camera.ToggleFPS();
	}
	if (!input.IsCursorCaptured())
	{
		return;
	}
	if (input.IsDown(GLFW_KEY_W)) m_Camera.CameraMovement(FORWARD, DeltaTime);
	if (input.IsDown(GLFW_KEY_S)) m_Camera.CameraMovement(BACKWARD, DeltaTime);
	if (input.IsDown(GLFW_KEY_A)) m_Camera.CameraMovement(LEFT, DeltaTime);
	if (input.IsDown(GLFW_KEY_D)) m_Camera.CameraMovement(RIGHT, DeltaTime);
	if (input.IsDown(GLFW_KEY_Q)) m_Camera.CameraMovement(DOWN, DeltaTime);
	if (input.IsDown(GLFW_KEY_E)) m_Camera.CameraMovement(UP, DeltaTime);

	if (input.IsDown(GLFW_KEY_KP_ADD)) m_Camera.AdjustFOV(IN);
	if (input.IsDown(GLFW_KEY_KP_SUBTRACT)) m_Camera.AdjustFOV(OUT);
	if (input.IsDown(GLFW_KEY_KP_ENTER)) m_Camera.AdjustFOV(RESET);

	const glm::vec2 look = input.MouseDelta();
	m_Camera.PocessMouseMovement(look.x, look.y);
	m_Camera.ProcessMouseScroll(input.ScrollDelta().y);
}
