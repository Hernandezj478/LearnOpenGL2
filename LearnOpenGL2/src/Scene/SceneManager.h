#pragma once

#include <memory>
#include <string>

class Scene;
class SceneRegistry;
struct SceneContext;

class SceneManager
{
public:
	static constexpr int m_Home = -1;

	SceneManager(const SceneRegistry& Registry, const SceneContext& Context);
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	void RequestScene(int index) { m_Pending = index; }
	void RequestHome() { m_Pending = m_Home; }

	bool ApplyPending();

	void Update(float deltaTime);
	void Render();
	void OnResize(int width, int height);
	void OnGui();

	int CurrentIndex() const { return m_CurrentIndex; }
	bool IsHome() const { return m_CurrentIndex == m_Home; }
	const std::string& CurrentName() const { return m_CurrentName; }
	
	bool HasError() const { return !m_LastError.empty(); }
	const std::string& LastError() const { return m_LastError; }
	void ClearError() { m_LastError.clear(); }

private:
	static constexpr int m_NoRequest = -2;

	bool TryLoad(int index);

	const SceneRegistry& m_Registry;
	const SceneContext& m_Context;

	std::unique_ptr<Scene> m_Current;
	int m_CurrentIndex = m_Home;
	int m_Pending = m_Home;
	std::string m_CurrentName;
	std::string m_LastError;
};