#pragma once

#include <array>
#include <string>

namespace IsoForge
{
struct EditorState
{
    int selectedDebugTileId = 1;
    std::string selectedTextureRelativePath;
    std::string selectedTextureDisplayName;
    std::array<std::string, 5> debugTileTexturePaths {};

    [[nodiscard]] bool HasSelectedTexture() const
    {
        return !selectedTextureRelativePath.empty();
    }

    [[nodiscard]] bool HasTextureBinding(int tileId) const
    {
        return tileId > 0 &&
               tileId < static_cast<int>(debugTileTexturePaths.size()) &&
               !debugTileTexturePaths[static_cast<size_t>(tileId)].empty();
    }

    void BindSelectedTextureToTile(int tileId)
    {
        if (!HasSelectedTexture() ||
            tileId <= 0 ||
            tileId >= static_cast<int>(debugTileTexturePaths.size()))
        {
            return;
        }

        debugTileTexturePaths[static_cast<size_t>(tileId)] = selectedTextureRelativePath;
    }

    void ClearTextureBinding(int tileId)
    {
        if (tileId <= 0 || tileId >= static_cast<int>(debugTileTexturePaths.size()))
        {
            return;
        }

        debugTileTexturePaths[static_cast<size_t>(tileId)].clear();
    }
};
}
