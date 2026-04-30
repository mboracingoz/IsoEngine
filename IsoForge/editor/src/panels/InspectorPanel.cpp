#include "editor/panels/InspectorPanel.h"

#include <imgui.h>

namespace IsoForge
{
InspectorPanel::InspectorPanel()
    : Panel("Inspector")
{
}

void InspectorPanel::OnImGuiRender()
{
    if (!m_IsOpen)
    {
        return;
    }

    if (ImGui::Begin(m_Title, &m_IsOpen))
    {
        ImGui::TextUnformatted(
            "Inspector will display selected entity components in a later phase."
        );
        ImGui::Separator();
        ImGui::TextUnformatted("No entity selected");
    }

    ImGui::End();
}
}
