#pragma once

#include "editor/ImGuiLayer.h"
#include "editor/panels/AssetBrowserPanel.h"
#include "editor/panels/ConsolePanel.h"
#include "editor/panels/HierarchyPanel.h"
#include "editor/panels/InspectorPanel.h"
#include "editor/panels/SceneViewportPanel.h"
#include "editor/panels/TilePalettePanel.h"
#include "engine/core/Application.h"

namespace IsoForge
{
class EditorApp : public Application
{
public:
    EditorApp();
    ~EditorApp() override = default;

protected:
    void OnInit() override;
    void OnEvent(const SDL_Event& event) override;
    void OnRender() override;
    void OnShutdown() override;

private:
    void RenderDockspace();
    void RenderAboutWindow();

    ImGuiLayer m_ImGuiLayer;
    SceneViewportPanel m_SceneViewportPanel;
    HierarchyPanel m_HierarchyPanel;
    InspectorPanel m_InspectorPanel;
    AssetBrowserPanel m_AssetBrowserPanel;
    TilePalettePanel m_TilePalettePanel;
    ConsolePanel m_ConsolePanel;
    bool m_ShowAboutWindow;
    bool m_ShowDemoWindow;
};
}
