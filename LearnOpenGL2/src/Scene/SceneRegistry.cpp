#include "SceneRegistry.h"

void SceneRegistry::AddEntry(SceneInfo info)
{
	const int index = static_cast<int>(m_Scenes.size());
	ChapterGroup* group = nullptr;
	for (ChapterGroup& candidate : m_Chapters)
	{
		if (candidate.Name == info.Chapter)
		{
			group = &candidate;
			break;
		}
	}
	if (group == nullptr)
	{
		m_Chapters.push_back(ChapterGroup{ info.Chapter, {} });
		group = &m_Chapters.back();
	}
	group->SceneIndices.push_back(index);
	m_Scenes.push_back(std::move(info));
}
