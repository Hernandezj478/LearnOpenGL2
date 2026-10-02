#pragma once

#include "Scene.h"

class WelcomeScene : public Scene
{
public:
	explicit WelcomeScene(const SceneContext& context) : Scene(context) {}

	void Render() override;
	void OnGui() override;
};