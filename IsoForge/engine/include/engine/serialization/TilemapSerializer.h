#pragma once

#include "engine/iso/TilemapData.h"

#include <filesystem>

namespace IsoForge
{
class TilemapSerializer
{
public:
    static bool SaveToFile(const TilemapData& tilemap, const std::filesystem::path& path);
    static bool LoadFromFile(TilemapData& tilemap, const std::filesystem::path& path);
};
}
