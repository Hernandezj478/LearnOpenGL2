#pragma once

#include "Scene/Scene3D.h"
#include "Graphics/LightMarker.h"

#include <memory>

class Shader;
class Cube;

class Colors : public Scene3D
{
public:
	explicit Colors(const SceneContext& context);
	~Colors();

	void Render() override;

private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Cube> m_Cube;

	LightMarker m_LightMarker;

};