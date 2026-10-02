#include "MenuUI.h"

#include <imgui/imgui.h>
#include "Scene/SceneRegistry.h"
#include "Scene/SceneManager.h"

namespace
{
    constexpr float kItemWidth = 320.f;
}

MenuUI::MenuUI(const glm::ivec2& MaxResolution)
{
    const Resolution presets[] =
    {
        {"1280 x 720",  1280, 720},
        {"1600 x 900",  1600, 900},
        {"1920 x 1080", 1920, 1080},
        {"2560 X 1440", 2560, 1440},
        {"3840 x 2160", 3840, 2160}
    };
    for (const Resolution& preset : presets)
    {
        if (preset.Width <= MaxResolution.x && preset.Height <= MaxResolution.y)
        {
            m_Resolutions.push_back(preset);
        }
    }
}

void MenuUI::OnEscape()
{
    switch (m_Page)
    {
    case MenuPage::Hidden:
        m_Page = MenuPage::Root; 
        break;
    case MenuPage::Root:
    case MenuPage::Chapters:
    case MenuPage::Settings:
        m_Page = MenuPage::Hidden;
        break;
    }
}

UIRequests MenuUI::Draw(const glm::ivec2 & FramebufferSize, const SceneRegistry& Registry, int CurrentScene)
{
    UIRequests requests;
    if (m_Page == MenuPage::Hidden)
    {
        if (CurrentScene == SceneManager::m_Home)
        {
            DrawHomePanel(requests, Registry, CurrentScene);
        }
        return requests;
    }
    DrawBackdrop();
    // Keep the window cenetered. The pivot (0.5, 0.5) means the position is the window's center,
    // and reapplying it every frame keeps it centered when the page changes
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings;
    if (ImGui::Begin("Menu", nullptr, flags))
    {
        switch (m_Page)
        {
        case MenuPage::Root: 
            DrawRoot(requests, FramebufferSize);
            break;
        case MenuPage::Chapters:
            DrawChapters(requests, Registry, CurrentScene);
            break;
        case MenuPage::Settings:
            DrawSettings(requests, FramebufferSize);
            break;
        default:
            break;
        }
        ImGui::End();
        return requests;
    }
    return UIRequests();
}

void MenuUI::DrawBackdrop()
{
    // The background draw list is drawn above the scene but below every ImGui window, so a translucent rectangle dims the scene
    // without covering the menu
    ImDrawList* DrawList = ImGui::GetBackgroundDrawList();
    DrawList->AddRectFilled(ImVec2(0.0f, 0.0f), ImGui::GetIO().DisplaySize, IM_COL32(0, 0, 0, 140));
}

void MenuUI::DrawRoot(UIRequests& Requests, const glm::ivec2 & FramebufferSize)
{
    const ImVec2 ButtonSize(kItemWidth, 0.0f);
    if (ImGui::Button("Home", ButtonSize))
    {
        Requests.GoHome = true;
        Close();
    }
    if (ImGui::Button("Chapters", ButtonSize))
    {
        m_Page = MenuPage::Chapters;
    }
    if (ImGui::Button("Settings", ButtonSize))
    {
        m_Page = MenuPage::Settings;
    }
    
    ImGui::Separator();
    if (ImGui::Button("Quit", ButtonSize))
    {
        Requests.Quit = true;
    }
}

void MenuUI::DrawChapters(UIRequests & Requests, const SceneRegistry& Registry, int CurrentScene)
{
    const std::vector<ChapterGroup>& chapters = Registry.Chapters();
    ImGui::Text("Chapters");
    ImGui::Separator();

    DrawChapterPicker(Requests, Registry, CurrentScene);
    
    ImGui::Separator();
    if (ImGui::Button("Back", ImVec2(kItemWidth, 0)))
    {
        m_Page = MenuPage::Root;
    }
}

void MenuUI::DrawSettings(UIRequests& Requests, const glm::ivec2& FramebufferSize)
{
    ImGui::Text("Resolution");
    ImGui::SameLine();
    ImGui::TextDisabled("(Current: %d x %d)", FramebufferSize.x, FramebufferSize.y);

    const char* preview = (m_SelectedResolution >= 0) ? m_Resolutions[m_SelectedResolution].label : "Current";
    ImGui::SetNextItemWidth(kItemWidth);
    if (ImGui::BeginCombo("##Resolution", preview))
    {
        for (int i = 0; i < static_cast<int>(m_Resolutions.size()); i++)
        {
            const bool isSelected = (i == m_SelectedResolution);
            if (ImGui::Selectable(m_Resolutions[i].label, isSelected))
            {
                m_SelectedResolution = i;
                Requests.Resize = glm::ivec2(m_Resolutions[i].Width, m_Resolutions[i].Height);
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Separator();
    if (ImGui::Button("Back", ImVec2(kItemWidth, 0)))
    {
        m_Page = MenuPage::Root;
    }
}

void MenuUI::DrawChapterPicker(UIRequests& requests, const SceneRegistry& registry, int currentScene)
{
    const std::vector<ChapterGroup>& chapters = registry.Chapters();
    if (chapters.empty())
    {
        ImGui::TextDisabled("No sections registered yet.");
        return;
    }
    if (m_SelectedChapter < 0 || m_SelectedChapter >= static_cast<int>(chapters.size()))
    {
        m_SelectedChapter = 0;
    }
    ImGui::BeginChild("ChapterList", ImVec2(180.f, 260.f), true);
    for (int c = 0; c < static_cast<int>(chapters.size()); c++)
    {
        if (ImGui::Selectable(chapters[c].Name.c_str(), c == m_SelectedChapter))
        {
            m_SelectedChapter = c;
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    ImGui::BeginChild("SectionList", ImVec2(280.f, 260.f), true);
    for (int sceneIndex : chapters[m_SelectedChapter].SceneIndices)
    {
        const SceneInfo& info = registry.Get(sceneIndex);
        ImGui::PushID(sceneIndex);
        if (ImGui::Selectable(info.Name.c_str(), sceneIndex == currentScene))
        {
            requests.SceneIndex = sceneIndex;
            Close();
        }
        if (ImGui::IsItemHovered() && !info.Description.empty())
        {
            ImGui::SetTooltip("%s", info.Description.c_str());
        }
        ImGui::PopID();
    }
    ImGui::EndChild();
}

void MenuUI::DrawHomePanel(UIRequests & requests, const SceneRegistry & registry, int currentScene)
{
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f + 12.0f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.0f, 0.5f));
    const ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize \
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings;
    if (ImGui::Begin("Chapters", nullptr, flags))
    {
        DrawChapterPicker(requests, registry, currentScene);
    }
    ImGui::End();
}
