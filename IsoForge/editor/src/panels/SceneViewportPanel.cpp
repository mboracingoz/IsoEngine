#include "editor/panels/SceneViewportPanel.h"

#include "engine/serialization/TilemapSerializer.h"

#include <imgui.h>

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <type_traits>

namespace IsoForge
{
namespace
{
struct DebugTileColor
{
    float r;
    float g;
    float b;
    float a;
};

template <typename TextureID = ImTextureID>
TextureID ToImTextureID(uint32_t textureID)
{
    if constexpr (std::is_pointer_v<TextureID>)
    {
        return reinterpret_cast<TextureID>(static_cast<intptr_t>(textureID));
    }
    else
    {
        return static_cast<TextureID>(textureID);
    }
}

DebugTileColor GetDebugTileColor(int tileId)
{
    switch (tileId)
    {
    case 1:
        return {0.18f, 0.45f, 0.75f, 0.45f};
    case 2:
        return {0.20f, 0.70f, 0.35f, 0.45f};
    case 3:
        return {0.85f, 0.55f, 0.20f, 0.45f};
    case 4:
        return {0.75f, 0.25f, 0.65f, 0.45f};
    default:
        return {0.50f, 0.50f, 0.50f, 0.45f};
    }
}
}

SceneViewportPanel::SceneViewportPanel(EditorState& editorState)
    : Panel("Scene Viewport")
    , m_EditorState(editorState)
    , m_Framebuffer(FramebufferSpec{1280, 720})
{
}

bool SceneViewportPanel::SaveTilemap()
{
    if (m_CurrentTilemapPath.empty())
    {
        m_LastTilemapIOStatus = "Tilemap IO: Save failed (no current tilemap path)";
        return false;
    }

    return SaveTilemapAs(m_CurrentTilemapPath);
}

bool SceneViewportPanel::SaveTilemap(const std::filesystem::path& path)
{
    return SaveTilemapAs(path);
}

bool SceneViewportPanel::SaveTilemapAs(const std::filesystem::path& path)
{
    const bool saved = TilemapSerializer::SaveToFile(m_Tilemap, path);
    if (saved)
    {
        m_CurrentTilemapPath = path;
        m_LastTilemapIOStatus = "Tilemap IO: Saved to " + path.generic_string();
    }
    else
    {
        m_LastTilemapIOStatus = "Tilemap IO: Save failed";
    }

    return saved;
}

bool SceneViewportPanel::LoadTilemap(const std::filesystem::path& path)
{
    const bool loaded = TilemapSerializer::LoadFromFile(m_Tilemap, path);
    if (loaded)
    {
        m_CurrentTilemapPath = path;
        m_LastTilemapIOStatus = "Tilemap IO: Loaded from " + path.generic_string();
    }
    else
    {
        m_LastTilemapIOStatus = "Tilemap IO: Load failed";
    }

    return loaded;
}

bool SceneViewportPanel::HasCurrentTilemapPath() const
{
    return !m_CurrentTilemapPath.empty();
}

const std::filesystem::path& SceneViewportPanel::GetCurrentTilemapPath() const
{
    return m_CurrentTilemapPath;
}

const std::string& SceneViewportPanel::GetLastTilemapIOStatus() const
{
    return m_LastTilemapIOStatus;
}

std::filesystem::path SceneViewportPanel::GetTilemapDirectory()
{
    return "sandbox_project/tilemaps";
}

void SceneViewportPanel::OnImGuiRender()
{
    if (!m_IsOpen)
    {
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin(m_Title, &m_IsOpen))
    {
        bool viewportHovered = false;
        const bool viewportFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        ImVec2 imageMin = ImVec2(0.0f, 0.0f);
        ImVec2 imageMax = ImVec2(0.0f, 0.0f);
        Vec2 viewportLocalMouse {};
        HoverResult currentHoverResult {};

        if (ImGui::Button("Reset Camera"))
        {
            m_EditorCamera.Reset();
        }

        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (viewportSize.x > 0.0f && viewportSize.y > 0.0f)
        {
            const uint32_t width = static_cast<uint32_t>(viewportSize.x);
            const uint32_t height = static_cast<uint32_t>(viewportSize.y);
            const float viewportWidth = static_cast<float>(width);
            const float viewportHeight = static_cast<float>(height);
            const float baseOriginX = viewportWidth * 0.5f;
            const float baseOriginY = viewportHeight * 0.15f;
            const float zoom = m_EditorCamera.GetZoom();
            const float originX = baseOriginX + m_EditorCamera.GetOffsetX();
            const float originY = baseOriginY + m_EditorCamera.GetOffsetY();
            const float tileWidth = 64.0f * zoom;
            const float tileHeight = 32.0f * zoom;

            m_Framebuffer.Resize(width, height);
            m_Framebuffer.Bind();
            m_Framebuffer.Clear(0.08f, 0.10f, 0.14f, 1.0f);
            for (int y = 0; y < m_Tilemap.GetRows(); ++y)
            {
                for (int x = 0; x < m_Tilemap.GetColumns(); ++x)
                {
                    const int tileId = m_Tilemap.GetTile(x, y);
                    if (tileId != TilemapData::EmptyTile)
                    {
                        const DebugTileColor color = GetDebugTileColor(tileId);
                        m_IsoGridRenderer.DrawFilledTile(
                            x,
                            y,
                            tileWidth,
                            tileHeight,
                            originX,
                            originY,
                            viewportWidth,
                            viewportHeight,
                            color.r,
                            color.g,
                            color.b,
                            color.a
                        );
                    }
                }
            }
            m_IsoGridRenderer.DrawGrid(
                m_Tilemap.GetColumns(),
                m_Tilemap.GetRows(),
                tileWidth,
                tileHeight,
                originX,
                originY,
                viewportWidth,
                viewportHeight
            );
            if (m_HoverResult.isValid)
            {
                m_IsoGridRenderer.DrawTileHighlight(
                    m_HoverResult.tile.x,
                    m_HoverResult.tile.y,
                    tileWidth,
                    tileHeight,
                    originX,
                    originY,
                    viewportWidth,
                    viewportHeight
                );
            }
            m_Framebuffer.Unbind();

            ImGui::Image(
                ToImTextureID(m_Framebuffer.GetColorAttachmentID()),
                viewportSize,
                ImVec2(0.0f, 1.0f),
                ImVec2(1.0f, 0.0f)
            );

            viewportHovered = ImGui::IsItemHovered();
            imageMin = ImGui::GetItemRectMin();
            imageMax = ImGui::GetItemRectMax();
            const ImVec2 mousePos = ImGui::GetMousePos();
            viewportLocalMouse = {
                mousePos.x - imageMin.x,
                mousePos.y - imageMin.y
            };
            ImGuiIO& io = ImGui::GetIO();

            if ((viewportHovered || viewportFocused) && !io.WantTextInput && ImGui::IsKeyPressed(ImGuiKey_R, false))
            {
                m_EditorCamera.Reset();
            }

            if (viewportHovered)
            {
                if (io.MouseWheel != 0.0f)
                {
                    m_EditorCamera.Zoom(io.MouseWheel);
                }

                if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
                {
                    const ImVec2 delta = io.MouseDelta;
                    if (delta.x != 0.0f || delta.y != 0.0f)
                    {
                        m_EditorCamera.Pan(delta.x, delta.y);
                    }
                }
            }

            if (viewportHovered)
            {
                currentHoverResult = IsoMath::GetHoveredTile(
                    viewportLocalMouse,
                    originX,
                    originY,
                    1.0f,
                    tileWidth,
                    tileHeight,
                    m_Tilemap.GetColumns(),
                    m_Tilemap.GetRows()
                );
            }

            const bool middleMouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
            if (viewportHovered && currentHoverResult.isValid && !middleMouseDown)
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    m_Tilemap.SetTile(
                        currentHoverResult.tile.x,
                        currentHoverResult.tile.y,
                        m_EditorState.selectedDebugTileId
                    );
                }

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                    m_Tilemap.ClearTile(currentHoverResult.tile.x, currentHoverResult.tile.y);
                }
            }

            m_HoverResult = currentHoverResult;
        }
        else
        {
            m_HoverResult = {};
            ImGui::TextUnformatted("Scene viewport is too small to display the framebuffer.");
        }

        char offsetBuffer[128] = {};
        std::snprintf(
            offsetBuffer,
            sizeof(offsetBuffer),
            "Camera Offset: %.1f, %.1f",
            m_EditorCamera.GetOffsetX(),
            m_EditorCamera.GetOffsetY()
        );
        ImGui::TextUnformatted(offsetBuffer);
        ImGui::Text("Zoom: %.2f", m_EditorCamera.GetZoom());
        ImGui::Text("Tilemap Size: %d x %d", m_Tilemap.GetColumns(), m_Tilemap.GetRows());
        ImGui::Text("Tile Count: %d", m_Tilemap.GetTileCount());
        ImGui::Text("Filled Tiles: %d", m_Tilemap.GetFilledTileCount());
        ImGui::Text("Empty Tile ID: %d", TilemapData::EmptyTile);
        ImGui::Text("Selected Debug Tile ID: %d", m_EditorState.selectedDebugTileId);
        ImGui::Text(
            "Current Tilemap Path: %s",
            m_CurrentTilemapPath.empty() ? "(none)" : m_CurrentTilemapPath.generic_string().c_str()
        );
        ImGui::Text("Tilemap Directory: %s", GetTilemapDirectory().generic_string().c_str());
        ImGui::TextUnformatted(m_LastTilemapIOStatus.c_str());
        ImGui::TextUnformatted("LMB: Paint selected debug tile");
        ImGui::TextUnformatted("RMB: Clear tile");
        ImGui::TextUnformatted("MMB Drag: Pan");
        ImGui::TextUnformatted("Mouse Wheel: Zoom");
        ImGui::TextUnformatted("R: Reset Camera");
        ImGui::Text("Viewport hovered: %s", viewportHovered ? "true" : "false");
        ImGui::Text("Viewport focused: %s", viewportFocused ? "true" : "false");
        ImGui::Text("Mouse local position: %.1f, %.1f", viewportLocalMouse.x, viewportLocalMouse.y);
        ImGui::Text("Hover valid: %s", currentHoverResult.isValid ? "true" : "false");
        ImGui::Text("Hovered tile: %d, %d", currentHoverResult.tile.x, currentHoverResult.tile.y);
        ImGui::Text(
            "Image Rect: Min(%.1f, %.1f) Max(%.1f, %.1f)",
            imageMin.x,
            imageMin.y,
            imageMax.x,
            imageMax.y
        );
    }

    ImGui::End();
    ImGui::PopStyleVar();
}
}
