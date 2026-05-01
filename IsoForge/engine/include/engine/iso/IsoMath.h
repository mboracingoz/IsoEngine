#pragma once

#include <cmath>

namespace IsoForge
{
struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
};

struct GridCoord
{
    int x = 0;
    int y = 0;
};

struct HoverResult
{
    bool isValid = false;
    GridCoord tile {};
};

namespace IsoMath
{
constexpr float DefaultTileWidth = 64.0f;
constexpr float DefaultTileHeight = 32.0f;

inline Vec2 GridToWorld(int gridX, int gridY, float tileWidth, float tileHeight)
{
    return {
        (static_cast<float>(gridX - gridY) * tileWidth * 0.5f),
        (static_cast<float>(gridX + gridY) * tileHeight * 0.5f)
    };
}

inline Vec2 ScreenToWorld(
    Vec2 screenPosition,
    float originX,
    float originY,
    float zoom
)
{
    if (zoom <= 0.0f)
    {
        return {};
    }

    return {
        (screenPosition.x - originX) / zoom,
        (screenPosition.y - originY) / zoom
    };
}

inline Vec2 WorldToGridFloat(
    Vec2 worldPosition,
    float tileWidth,
    float tileHeight
)
{
    if (tileWidth <= 0.0f || tileHeight <= 0.0f)
    {
        return {};
    }

    const float halfWidth = tileWidth * 0.5f;
    const float halfHeight = tileHeight * 0.5f;

    return {
        (worldPosition.y / halfHeight + worldPosition.x / halfWidth) * 0.5f,
        (worldPosition.y / halfHeight - worldPosition.x / halfWidth) * 0.5f
    };
}

inline GridCoord WorldToGrid(
    Vec2 worldPosition,
    float tileWidth,
    float tileHeight
)
{
    const Vec2 gridFloat = WorldToGridFloat(worldPosition, tileWidth, tileHeight);
    return {
        static_cast<int>(std::floor(gridFloat.x)),
        static_cast<int>(std::floor(gridFloat.y))
    };
}

inline HoverResult GetHoveredTile(
    Vec2 viewportLocalMouse,
    float originX,
    float originY,
    float zoom,
    float tileWidth,
    float tileHeight,
    int columns,
    int rows
)
{
    HoverResult result {};
    if (zoom <= 0.0f || tileWidth <= 0.0f || tileHeight <= 0.0f || columns <= 0 || rows <= 0)
    {
        return result;
    }

    const Vec2 worldPosition = ScreenToWorld(viewportLocalMouse, originX, originY, zoom);
    const GridCoord tile = WorldToGrid(worldPosition, tileWidth, tileHeight);

    if (tile.x >= 0 && tile.x < columns && tile.y >= 0 && tile.y < rows)
    {
        result.isValid = true;
        result.tile = tile;
    }

    return result;
}
}
}
