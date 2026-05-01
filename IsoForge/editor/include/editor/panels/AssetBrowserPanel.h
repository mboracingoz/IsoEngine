#pragma once

#include "editor/panels/Panel.h"

#include <filesystem>
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

private:
    std::filesystem::path m_TextureRootPath;
    std::vector<std::filesystem::path> m_TextureFiles;
};
}
