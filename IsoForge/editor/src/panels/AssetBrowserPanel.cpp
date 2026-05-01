#include "editor/panels/AssetBrowserPanel.h"

#include <imgui.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <string>
#include <type_traits>

namespace IsoForge
{
namespace
{
bool IsSupportedTextureFile(const std::filesystem::path& path)
{
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });

    return extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp";
}

template <typename TextureID = ImTextureID>
TextureID ToImTextureID(uint32_t rendererID)
{
    if constexpr (std::is_pointer_v<TextureID>)
    {
        return reinterpret_cast<TextureID>(static_cast<intptr_t>(rendererID));
    }
    else
    {
        return static_cast<TextureID>(rendererID);
    }
}
}

AssetBrowserPanel::AssetBrowserPanel()
    : Panel("Asset Browser")
    , m_AssetManager("sandbox_project")
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
        if (entry.is_regular_file() && IsSupportedTextureFile(entry.path()))
        {
            m_TextureFiles.push_back(entry.path().filename());
        }
    }

    std::sort(m_TextureFiles.begin(), m_TextureFiles.end());

    if (!m_SelectedTextureDisplayName.empty())
    {
        const auto selectedIt = std::find_if(
            m_TextureFiles.begin(),
            m_TextureFiles.end(),
            [this](const std::filesystem::path& file) {
                return file.filename().string() == m_SelectedTextureDisplayName;
            }
        );
        if (selectedIt == m_TextureFiles.end())
        {
            m_SelectedRelativeTexturePath.clear();
            m_SelectedTextureDisplayName.clear();
            m_SelectedPreviewTexture.reset();
            m_UsingMissingTextureFallback = false;
        }
    }
}

void AssetBrowserPanel::SelectTextureFile(const std::filesystem::path& fileName)
{
    m_SelectedTextureDisplayName = fileName.filename().string();
    m_SelectedRelativeTexturePath = std::filesystem::path("assets") / "textures" / fileName.filename();
    m_SelectedPreviewTexture = m_AssetManager.LoadTexture(m_SelectedRelativeTexturePath);
    m_UsingMissingTextureFallback = (m_SelectedPreviewTexture == m_AssetManager.GetMissingTexture());
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
                const bool isSelected = file.filename().string() == m_SelectedTextureDisplayName;
                if (ImGui::Selectable(file.filename().string().c_str(), isSelected))
                {
                    SelectTextureFile(file.filename());
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("[%s] %s", extension.c_str(), file.filename().string().c_str());
                }
            }
        }

        ImGui::Separator();

        if (m_SelectedTextureDisplayName.empty() || !m_SelectedPreviewTexture)
        {
            ImGui::TextUnformatted("Selected Texture: (none)");
        }
        else
        {
            ImGui::Text("Selected Texture: %s", m_SelectedTextureDisplayName.c_str());
            ImGui::Text("Relative Path: %s", m_SelectedRelativeTexturePath.generic_string().c_str());
            ImGui::Text(
                "Size: %d x %d",
                m_SelectedPreviewTexture->GetWidth(),
                m_SelectedPreviewTexture->GetHeight()
            );
            if (m_UsingMissingTextureFallback)
            {
                ImGui::TextUnformatted("Using missing texture fallback");
            }

            ImGui::TextUnformatted("Preview:");

            constexpr float maxPreviewExtent = 160.0f;
            const float textureWidth = static_cast<float>(m_SelectedPreviewTexture->GetWidth());
            const float textureHeight = static_cast<float>(m_SelectedPreviewTexture->GetHeight());
            const float previewScale = std::min(maxPreviewExtent / textureWidth, maxPreviewExtent / textureHeight);
            const ImVec2 previewSize(textureWidth * previewScale, textureHeight * previewScale);

            ImGui::Image(
                ToImTextureID(m_SelectedPreviewTexture->GetRendererID()),
                previewSize,
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f)
            );
        }
    }

    ImGui::End();
}
}
