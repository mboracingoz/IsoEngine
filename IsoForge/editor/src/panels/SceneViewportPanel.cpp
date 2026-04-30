#include "editor/panels/SceneViewportPanel.h"

#include <imgui.h>

namespace IsoForge
{
SceneViewportPanel::SceneViewportPanel()
    : Panel("Scene Viewport")
{
}

void SceneViewportPanel::OnImGuiRender()
{
    if (!m_IsOpen)
    {
        return;
    }

    if (ImGui::Begin(m_Title, &m_IsOpen))
    {
        ImGui::TextUnformatted("Scene viewport will display the framebuffer in a later phase.");
        ImGui::Separator();
        ImGui::TextUnformatted("Framebuffer: not implemented yet");
        ImGui::TextUnformatted("Renderer: not implemented yet");
    }

    ImGui::End();
}
}
