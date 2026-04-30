#include "editor/panels/AssetBrowserPanel.h"

#include <imgui.h>

namespace IsoForge
{
AssetBrowserPanel::AssetBrowserPanel()
    : Panel("Asset Browser")
{
}

void AssetBrowserPanel::OnImGuiRender()
{
    if (!m_IsOpen)
    {
        return;
    }

    if (ImGui::Begin(m_Title, &m_IsOpen))
    {
        ImGui::TextUnformatted("Asset browser will display project assets in a later phase.");
        ImGui::Separator();
        ImGui::TextUnformatted("Asset loading: not implemented yet");
    }

    ImGui::End();
}
}
