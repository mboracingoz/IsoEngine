#pragma once

namespace IsoForge
{
struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
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
}
}
