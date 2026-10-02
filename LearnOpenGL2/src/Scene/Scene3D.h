#pragma once

#include "Scene.h"
#include "Graphics/Camera.h"

class Scene3D : public Scene
{
public:
	void Update(float deltaTime) override;
	void OnResize(int width, int height) override;
protected:
	explicit Scene3D(const SceneContext& context);
	
	Camera m_Camera;
	float DeltaTime = 0.0f;
private:
	void UpdateCamera();
};