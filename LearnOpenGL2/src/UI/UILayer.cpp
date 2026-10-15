#include "UILayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "Scene/SceneManager.h"
#include "Scene/SceneRegistry.h"


UILayer::UILayer(const glm::ivec2& MaxResolution)
{
    const Resolution presets[] =
    {
        {" 800 x  600",  800,  600},
        {"1280 x  720", 1280,  720},
        {"1600 x  900", 1600,  900},
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

UIRequests UILayer::Draw(const SceneRegistry & Registry, SceneManager & SceneManager, const glm::ivec2 & FrameBufferSize, int FPS, double timeElapsed)
{
    const UIRequests requests = DrawMenuBar(Registry, SceneManager);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    m_DockspaceID = ImGui::GetID("MainDockspace");

    if (ImGui::DockBuilderGetNode(m_DockspaceID) == nullptr)
    {
        ImGui::DockBuilderAddNode(m_DockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::DockBuilderSetNodeSize(m_DockspaceID, viewport->Size);

        ImGuiID leftID = 0;
        ImGuiID centerID = m_DockspaceID;
        ImGui::DockBuilderSplitNode(centerID, ImGuiDir_Left, 0.125f, &leftID, &centerID);
        ImGui::DockBuilderDockWindow("Scene", leftID);

        ImGui::DockBuilderFinish(m_DockspaceID);
    }
    ImGui::DockSpaceOverViewport(m_DockspaceID, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
    
    DrawDataOverlay(FPS, timeElapsed);
    DrawScenePanael(SceneManager);
    
    return requests;
}

UIRequests UILayer::DrawMenuBar(const SceneRegistry & Registry, SceneManager & SceneManager)
{
    UIRequests requests;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if (ImGui::MenuItem("Home"))
            {
                requests.GoHome = true;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit"))
            {
                requests.Quit = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Chapters"))
        {
            for (const ChapterGroup& chapter : Registry.Chapters())
            {
                if (ImGui::BeginMenu(chapter.Name.c_str()))
                {
                    for (int sceneIndex : chapter.SceneIndices)
                    {
                        const SceneInfo& info = Registry.Get(sceneIndex);
                        const bool isCurrent = (sceneIndex == SceneManager.CurrentIndex());
                        if(ImGui::MenuItem(info.Name.c_str(), nullptr, isCurrent))
                        {
                            requests.SceneIndex = sceneIndex;
                        }
                        if (!info.Description.empty() && ImGui::IsItemHovered())
                        {
                            ImGui::SetTooltip("%s", info.Description.c_str());
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Settings"))
        {
            if (ImGui::BeginMenu("Resolution"))
            {
                for (const Resolution& res : m_Resolutions)
                {
                    if (ImGui::MenuItem(res.Label))
                    {
                        requests.Resize = glm::ivec2(res.Width, res.Height);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (SceneManager.HasError())
        {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0), "Load Failed: %s", SceneManager.LastError().c_str());
            ImGui::SameLine();
            if (ImGui::SmallButton("Dismiss"))
            {
                SceneManager.ClearError();
            }
        }
        ImGui::EndMainMenuBar();
    }
    return requests;
}

void UILayer::DrawDataOverlay(int FPS, double timeElapsed)
{
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 10.0f, 30.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoTitleBar;
    ImGui::SetNextWindowBgAlpha(0.1f);
    if (ImGui::Begin("##Data", nullptr, flags))
    {
        ImGui::Text("FPS: %d", FPS);
        ImGui::Text("%0.2f ms", timeElapsed);
    }
    ImGui::End();
}

void UILayer::DrawScenePanael(SceneManager & SceneManager)
{
    if (ImGui::Begin("Scene"))
    {
        ImGui::TextDisabled("%s", SceneManager.CurrentName().c_str());
        ImGui::Separator();
        SceneManager.OnGui();
    }
    ImGui::End();
}
