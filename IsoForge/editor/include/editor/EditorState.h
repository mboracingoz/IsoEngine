#pragma once

#include <string>

namespace IsoForge
{
struct EditorState
{
    int selectedDebugTileId = 1;
    std::string selectedTextureRelativePath;
    std::string selectedTextureDisplayName;
};
}
