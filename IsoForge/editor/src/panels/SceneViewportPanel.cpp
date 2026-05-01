#include "editor/panels/SceneViewportPanel.h"

#include <imgui.h>

#include <cstdint>
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
        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (viewportSize.x > 0.0f && viewportSize.y > 0.0f)
        {
            const uint32_t width = static_cast<uint32_t>(viewportSize.x);
            const uint32_t height = static_cast<uint32_t>(viewportSize.y);
            const float viewportWidth = static_cast<float>(width);
            const float viewportHeight = static_cast<float>(height);
            const float originX = viewportWidth * 0.5f;
            const float originY = 40.0f;

            m_Framebuffer.Resize(width, height);
            m_Framebuffer.Bind();
            m_Framebuffer.Clear(0.08f, 0.10f, 0.14f, 1.0f);
            m_IsoGridRenderer.DrawGrid(
                20,
                20,
                64.0f,
                32.0f,
                originX,
                originY,
                viewportWidth,
                viewportHeight
            );
            m_Framebuffer.Unbind();

            ImGui::Image(
                ToImTextureID(m_Framebuffer.GetColorAttachmentID()),
                viewportSize,
                ImVec2(0.0f, 1.0f),
                ImVec2(1.0f, 0.0f)
            );
        }
        else
        {
            ImGui::TextUnformatted("Scene viewport is too small to display the framebuffer.");
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
}
}
