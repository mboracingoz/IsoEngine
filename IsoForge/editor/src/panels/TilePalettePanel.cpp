#include "editor/panels/TilePalettePanel.h"

#include <imgui.h>

namespace IsoForge
{
TilePalettePanel::TilePalettePanel()
    : Panel("Tile Palette")
{
}

void TilePalettePanel::OnImGuiRender()
{
    if (!m_IsOpen)
    {
        return;
    }

    if (ImGui::Begin(m_Title, &m_IsOpen))
    {
        ImGui::TextUnformatted("Tile palette will be implemented when tilemap editing begins.");
        ImGui::Separator();
        ImGui::TextUnformatted("Selected tile: none");
    }

    ImGui::End();
}
}
