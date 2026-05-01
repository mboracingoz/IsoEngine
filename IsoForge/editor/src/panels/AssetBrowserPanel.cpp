#include "editor/panels/AssetBrowserPanel.h"

#include <imgui.h>

#include <algorithm>
#include <filesystem>
#include <string>

namespace IsoForge
{
AssetBrowserPanel::AssetBrowserPanel()
    : Panel("Asset Browser")
    , m_TextureRootPath("sandbox_project/assets/textures")
{
    RefreshTextureFileList();
}

void AssetBrowserPanel::RefreshTextureFileList()
{
    std::filesystem::create_directories(m_TextureRootPath);

    m_TextureFiles.clear();
    for (const auto& entry : std::filesystem::directory_iterator(m_TextureRootPath))
    {
        if (entry.is_regular_file())
        {
            m_TextureFiles.push_back(entry.path().filename());
        }
    }

    std::sort(m_TextureFiles.begin(), m_TextureFiles.end());
}

void AssetBrowserPanel::OnImGuiRender()
{
    if (!m_IsOpen)
    {
        return;
    }

    if (ImGui::Begin(m_Title, &m_IsOpen))
    {
        ImGui::TextUnformatted("Asset Browser");
        ImGui::Separator();
        ImGui::Text("Project Asset Root: %s", m_TextureRootPath.generic_string().c_str());

        if (ImGui::Button("Refresh"))
        {
            RefreshTextureFileList();
        }

        ImGui::Separator();

        if (m_TextureFiles.empty())
        {
            ImGui::TextUnformatted("No texture assets found.");
        }
        else
        {
            for (const auto& file : m_TextureFiles)
            {
                const std::string extension = file.extension().string();
                ImGui::BulletText("[%s] %s", extension.c_str(), file.filename().string().c_str());
            }
        }
    }

    ImGui::End();
}
}
