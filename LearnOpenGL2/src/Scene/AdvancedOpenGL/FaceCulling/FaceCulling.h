#pragma once

#include "Scene/Scene3D.h"
#include <memory>
#include <string>
#include <map>

class Cube;
class Texture;
class Shader;

class FaceCulling : public Scene3D
{
public:
	FaceCulling(const SceneContext& context);
	~FaceCulling();

	void Render() override;
	void OnGui() override;
private:
	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Texture> m_Texture;
	int m_FaceCullSelection = GL_FRONT;
	int m_WindingSelection = GL_CCW;
	std::map<int, std::string> m_CullMap;
	std::map<int, std::string> m_WindMap;
};
