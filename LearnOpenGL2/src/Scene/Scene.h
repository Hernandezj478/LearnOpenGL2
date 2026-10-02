#pragma once

#include <stdexcept>

struct SceneContext;

class Scene
{
public:
	virtual ~Scene() = default;
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	virtual void Render() = 0;
	virtual void Update(float deltaTime){}
	virtual void OnResize(int width, int height) {}
	virtual void OnGui() {}
protected:
	explicit Scene(const SceneContext& context) : m_Context(context) {}
	const SceneContext& m_Context;
};