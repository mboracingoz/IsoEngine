#pragma once

#include "editor/EditorState.h"
#include "editor/ImGuiLayer.h"
#include "editor/panels/AssetBrowserPanel.h"
#include "editor/panels/ConsolePanel.h"
#include "editor/panels/HierarchyPanel.h"
#include "editor/panels/InspectorPanel.h"
#include "editor/panels/SceneViewportPanel.h"
#include "editor/panels/TilePalettePanel.h"
#include "engine/core/Application.h"

#include <array>
#include <filesystem>
#include <string>
#include <vector>

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
    void RenderTilemapSaveAsPopup();
    void RenderTilemapLoadPopup();
    void RenderAboutWindow();

    EditorState m_EditorState;
    ImGuiLayer m_ImGuiLayer;
    SceneViewportPanel m_SceneViewportPanel;
    HierarchyPanel m_HierarchyPanel;
    InspectorPanel m_InspectorPanel;
    AssetBrowserPanel m_AssetBrowserPanel;
    TilePalettePanel m_TilePalettePanel;
    ConsolePanel m_ConsolePanel;
    std::array<char, 128> m_TilemapSaveNameBuffer {};
    std::vector<std::filesystem::path> m_AvailableTilemapFiles;
    std::string m_TilemapPopupMessage;
    bool m_OpenSaveTilemapAsPopup = false;
    bool m_OpenLoadTilemapPopup = false;
    bool m_ShowAboutWindow;
    bool m_ShowDemoWindow;
};
}
