#include "editor/panels/HierarchyPanel.h"

#include <imgui.h>

namespace IsoForge
{
HierarchyPanel::HierarchyPanel()
    : Panel("Hierarchy")
{
}

void HierarchyPanel::OnImGuiRender()
{
    if (!m_IsOpen)
    {
        return;
    }

    if (ImGui::Begin(m_Title, &m_IsOpen))
    {
        ImGui::TextUnformatted("Scene hierarchy will be implemented after the ECS scene system.");
        ImGui::Separator();
        ImGui::TextUnformatted("No scene loaded");
    }

    ImGui::End();
}
}
