#pragma once

#include <future>
#include <vector>
#include <functional>
#include <chrono>

template <typename T>
class AsyncLoader
{
public:
	void Request(std::function<T()> work)
	{
		m_Pending.push_back(std::async(std::launch::async, std::move(work)));
	}

	void Update(const std::function<void(T)>& OnReady, const std::function<void(const std::exception&)>& OnError = nullptr)
	{
		for (auto it = m_Pending.begin(); it != m_Pending.end();)
		{
			if (IsFinished(*it))
			{
				Collect(*it, OnReady, OnError);
				it = m_Pending.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	bool HasPending() const { return !m_Pending.empty(); }

private:
	static bool IsFinished(const std::future<T>& f)
	{
		return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
	
	static void Collect(std::future<T>& f,
		const std::function<void(T)>& OnReady,
		const std::function<void(const std::exception&)>& OnError)
	{
		try
		{
			OnReady(f.get());
		}
		catch (const std::exception& e)
		{
			if (OnError) OnError(e);
		}
	}

	std::vector<std::future<T>> m_Pending;
};