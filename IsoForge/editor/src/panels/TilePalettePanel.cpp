#include "editor/panels/TilePalettePanel.h"

#include <imgui.h>

#include <string>

namespace IsoForge
{
namespace
{
void DrawBindingRow(EditorState& editorState, int tileId)
{
    const std::string& bindingPath = editorState.debugTileTexturePaths[static_cast<size_t>(tileId)];

    ImGui::Text("Tile %d: %s", tileId, bindingPath.empty() ? "(none)" : bindingPath.c_str());

    const bool hasSelectedTexture = editorState.HasSelectedTexture();
    ImGui::BeginDisabled(!hasSelectedTexture);
    const std::string bindButtonLabel = "Bind Selected Texture##Tile" + std::to_string(tileId);
    if (ImGui::Button(bindButtonLabel.c_str()))
    {
        editorState.BindSelectedTextureToTile(tileId);
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    const std::string clearButtonLabel = "Clear Binding##Tile" + std::to_string(tileId);
    if (ImGui::Button(clearButtonLabel.c_str()))
    {
        editorState.ClearTextureBinding(tileId);
    }
}
}

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
        ImGui::TextUnformatted("Texture asset selection is preview-only in this phase.");
        ImGui::TextUnformatted("Debug painting still uses numeric tile IDs.");
        ImGui::Spacing();

        ImGui::TextUnformatted("Selected Texture Asset");
        if (m_EditorState.selectedTextureRelativePath.empty())
        {
            ImGui::TextUnformatted("None selected");
            ImGui::TextUnformatted("Select a texture in Asset Browser.");
        }
        else
        {
            ImGui::Text("Display Name: %s", m_EditorState.selectedTextureDisplayName.c_str());
            ImGui::Text("Relative Path: %s", m_EditorState.selectedTextureRelativePath.c_str());
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Debug Tile Texture Bindings");
        if (!m_EditorState.HasSelectedTexture())
        {
            ImGui::TextUnformatted("Select a texture in Asset Browser to enable binding.");
        }

        DrawBindingRow(m_EditorState, 1);
        DrawBindingRow(m_EditorState, 2);
        DrawBindingRow(m_EditorState, 3);
        DrawBindingRow(m_EditorState, 4);

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
