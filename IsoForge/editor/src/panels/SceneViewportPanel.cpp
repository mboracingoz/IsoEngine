#include "editor/panels/SceneViewportPanel.h"

#include <imgui.h>

#include <cstdint>
#include <cstdio>
#include <type_traits>

namespace IsoForge
{
namespace
{
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
}

SceneViewportPanel::SceneViewportPanel()
    : Panel("Scene Viewport")
    , m_Framebuffer(FramebufferSpec{1280, 720})
{
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
        ImGui::TextUnformatted("Shortcut: R = Reset Camera");
        ImGui::Text("Tilemap Size: %d x %d", m_Tilemap.GetColumns(), m_Tilemap.GetRows());
        ImGui::Text("Tile Count: %d", m_Tilemap.GetTileCount());
        ImGui::Text("Filled Tiles: %d", m_Tilemap.GetFilledTileCount());
        ImGui::Text("Empty Tile ID: %d", TilemapData::EmptyTile);

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

            m_HoverResult = currentHoverResult;
        }
        else
        {
            m_HoverResult = {};
            ImGui::TextUnformatted("Scene viewport is too small to display the framebuffer.");
        }

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
