#include "engine/serialization/TilemapSerializer.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

namespace IsoForge
{
namespace
{
constexpr int SupportedVersion = 1;
}

bool TilemapSerializer::SaveToFile(const TilemapData& tilemap, const std::filesystem::path& path)
{
    try
    {
        const std::filesystem::path parentPath = path.parent_path();
        if (!parentPath.empty())
        {
            std::filesystem::create_directories(parentPath);
        }

        nlohmann::json json;
        json["version"] = SupportedVersion;
        json["columns"] = tilemap.GetColumns();
        json["rows"] = tilemap.GetRows();
        json["empty_tile"] = TilemapData::EmptyTile;
        json["tiles"] = tilemap.GetTiles();

        std::ofstream output(path);
        if (!output.is_open())
        {
            std::cerr << "Failed to open tilemap file for save: " << path << '\n';
            return false;
        }

        output << json.dump(4);
        if (!output.good())
        {
            std::cerr << "Failed to write tilemap file: " << path << '\n';
            return false;
        }

        return true;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Failed to save tilemap JSON: " << exception.what() << '\n';
        return false;
    }
}

bool TilemapSerializer::LoadFromFile(TilemapData& tilemap, const std::filesystem::path& path)
{
    try
    {
        std::ifstream input(path);
        if (!input.is_open())
        {
            std::cerr << "Failed to open tilemap file for load: " << path << '\n';
            return false;
        }

        const nlohmann::json json = nlohmann::json::parse(input);

        const int version = json.value("version", SupportedVersion);
        if (version != SupportedVersion)
        {
            std::cerr << "Unsupported tilemap JSON version: " << version << '\n';
            return false;
        }

        const int columns = json.value("columns", 0);
        const int rows = json.value("rows", 0);
        if (columns <= 0 || rows <= 0)
        {
            std::cerr << "Invalid tilemap dimensions in JSON: " << columns << "x" << rows << '\n';
            return false;
        }

        if (!json.contains("tiles") || !json["tiles"].is_array())
        {
            std::cerr << "Tilemap JSON is missing a valid tiles array." << '\n';
            return false;
        }

        const std::vector<int> tiles = json["tiles"].get<std::vector<int>>();
        const size_t expectedTileCount = static_cast<size_t>(columns) * static_cast<size_t>(rows);
        if (tiles.size() != expectedTileCount)
        {
            std::cerr << "Tilemap JSON tile count mismatch. Expected "
                      << expectedTileCount
                      << ", got "
                      << tiles.size()
                      << '\n';
            return false;
        }

        if (!tilemap.ReplaceTiles(columns, rows, tiles))
        {
            std::cerr << "Failed to replace tilemap data after JSON load." << '\n';
            return false;
        }

        return true;
    }
    catch (const nlohmann::json::exception& exception)
    {
        std::cerr << "Failed to parse tilemap JSON: " << exception.what() << '\n';
        return false;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Failed to load tilemap JSON: " << exception.what() << '\n';
        return false;
    }
}
}
