#pragma once

#include <glm/glm.hpp>
#include <memory>

class Shader;
class Sphere;
class Renderer;

class LightMarker
{
public:
	LightMarker();
	~LightMarker();
	LightMarker(const LightMarker&) = delete;
	LightMarker& operator=(const LightMarker&) = delete;

	void Draw(const Renderer& renderer, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position, 
		const glm::vec3& color, float brightness = 1.0f, float scale = 0.05f);
private:
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Sphere> m_Light;
};