#pragma once

#include "Scene.h"

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using SceneFactory = std::function<std::unique_ptr<Scene>(const SceneContext&)>;

struct SceneInfo
{
	std::string Chapter;
	std::string Name;
	std::string Description;
	SceneFactory Create;
};

struct ChapterGroup
{
	std::string Name;
	std::vector<int> SceneIndices;
};

class SceneRegistry
{
public:
	/*
	*! @brief Registers the welcome scene. Call only once, before Application::Run 
	*/
	template<class T>
	void RegisterHome(std::string name = "Welcome")
	{
		m_Home = SceneInfo{ "", std::move(name), "", MakeFactory<T>() };
	}
	template<class T>
	void Register(std::string chapter, std::string name, std::string description = "")
	{
		AddEntry(SceneInfo{ std::move(chapter), std::move(name), std::move(description), MakeFactory<T>() });
	}
	bool HasHome() const { return static_cast<bool>(m_Home.Create); }
	const SceneInfo& Home() const { return m_Home; }

	int Count() const { return static_cast<int>(m_Scenes.size()); }
	const SceneInfo& Get(int index) const { return m_Scenes.at(static_cast<size_t>(index)); }

	const std::vector<ChapterGroup>& Chapters() const { return m_Chapters; }

private:
	template<class T>
	static SceneFactory MakeFactory()
	{
		return [](const SceneContext& context) -> std::unique_ptr<Scene>
		{
			return std::make_unique<T>(context);
		};
	}
	void AddEntry(SceneInfo info);
	SceneInfo m_Home;
	std::vector<SceneInfo> m_Scenes;
	std::vector<ChapterGroup> m_Chapters;
};