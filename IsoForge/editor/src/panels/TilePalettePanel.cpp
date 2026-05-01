#include "editor/panels/TilePalettePanel.h"

#include <imgui.h>

namespace IsoForge
{
TilePalettePanel::TilePalettePanel(EditorState& editorState)
    : Panel("Tile Palette")
    , m_EditorState(editorState)
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
        ImGui::TextUnformatted("Debug Tile Palette");
        ImGui::Separator();
        ImGui::TextUnformatted("Texture tiles are not implemented yet.");
        ImGui::Spacing();

        bool isSelected = (m_EditorState.selectedDebugTileId == 1);
        if (ImGui::Selectable("Tile 1", isSelected))
        {
            m_EditorState.selectedDebugTileId = 1;
        }

        isSelected = (m_EditorState.selectedDebugTileId == 2);
        if (ImGui::Selectable("Tile 2", isSelected))
        {
            m_EditorState.selectedDebugTileId = 2;
        }

        isSelected = (m_EditorState.selectedDebugTileId == 3);
        if (ImGui::Selectable("Tile 3", isSelected))
        {
            m_EditorState.selectedDebugTileId = 3;
        }

        isSelected = (m_EditorState.selectedDebugTileId == 4);
        if (ImGui::Selectable("Tile 4", isSelected))
        {
            m_EditorState.selectedDebugTileId = 4;
        }

        ImGui::Separator();
        ImGui::Text("Selected Debug Tile ID: %d", m_EditorState.selectedDebugTileId);
    }

    ImGui::End();
}
}
