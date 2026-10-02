#pragma once

#include <filesystem>

class Window;
class Input;
class Renderer;

struct SceneContext
{
	const Window& Window;
	const Input& Input;
	const Renderer& Renderer;
	std::filesystem::path AssetRoot;
	// AssetCache& Assets;
};