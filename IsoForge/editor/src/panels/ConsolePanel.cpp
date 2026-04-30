#include "editor/panels/ConsolePanel.h"

#include <imgui.h>

namespace IsoForge
{
ConsolePanel::ConsolePanel()
    : Panel("Console")
{
}

void ConsolePanel::OnImGuiRender()
{
    if (!m_IsOpen)
    {
        return;
    }

    if (ImGui::Begin(m_Title, &m_IsOpen))
    {
        ImGui::TextUnformatted("Console log sink will be implemented in a later phase.");
        ImGui::Separator();
        ImGui::TextUnformatted("No logs yet");
    }

    ImGui::End();
}
}
