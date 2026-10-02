#include "SceneManager.h"

#include "Graphics/GLState.h"
#include "Scene.h"
#include "SceneContext.h"
#include "SceneRegistry.h"

#include <exception>
#include <stdexcept>

SceneManager::SceneManager(const SceneRegistry& Registry, const SceneContext& Context) : m_Registry(Registry), m_Context(Context)
{}

SceneManager::~SceneManager() = default;

bool SceneManager::ApplyPending()
{
	if (m_Pending == m_NoRequest)
	{
		return false;
	}
	const int target = m_Pending;
	m_Pending = m_NoRequest;
	m_LastError.clear();
	m_Current.reset();
	m_CurrentIndex = m_Home;
	m_CurrentName.clear();
	ResetGLState();

	if (!TryLoad(target))
	{
		ResetGLState();
		TryLoad(m_Home);
	}
	return true;
}

void SceneManager::Update(float deltaTime)
{
	if (m_Current)
	{
		m_Current->Update(deltaTime);
	}
}

void SceneManager::Render()
{
	if (m_Current)
	{
		m_Current->Render();
	}
}

void SceneManager::OnResize(int width, int height)
{
	if (m_Current)
	{
		m_Current->OnResize(width, height);
	}
}

void SceneManager::OnGui()
{
	if (m_Current)
	{
		m_Current->OnGui();
	}
}

bool SceneManager::TryLoad(int index)
{
	std::string message;
	try
	{
		const SceneInfo& info = (index == m_Home) ? m_Registry.Home() : m_Registry.Get(index);
		if (!info.Create)
		{
			throw std::runtime_error(index == m_Home 
				? "No welcome scene registered (call RegisterHome brfore Run)" 
				: "Scene has no factory");
		}
		m_Current = info.Create(m_Context);
		m_CurrentIndex = index;
		m_CurrentName = info.Name;
		return true;
	}
	catch (const std::exception& e)
	{
		message = e.what();
	}
	catch (...)
	{
		message = "Unknown error while loading the scene.";
	}
	if (index == m_Home)
	{
		throw std::runtime_error("Failed to load the Welcome Scene: " + message);
	}
	m_LastError = message;
	return false;
}
