#pragma once

#include "editor/panels/Panel.h"
#include "engine/assets/AssetManager.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace IsoForge
{
class AssetBrowserPanel : public Panel
{
public:
    AssetBrowserPanel();
    void OnImGuiRender() override;

private:
    void RefreshTextureFileList();
    void SelectTextureFile(const std::filesystem::path& fileName);

private:
    AssetManager m_AssetManager;
    std::filesystem::path m_TextureRootPath;
    std::vector<std::filesystem::path> m_TextureFiles;
    std::filesystem::path m_SelectedRelativeTexturePath;
    std::string m_SelectedTextureDisplayName;
    std::shared_ptr<Texture2D> m_SelectedPreviewTexture;
    bool m_UsingMissingTextureFallback = false;
};
}
