#pragma once

#include <cstdint>

namespace IsoForge
{
class IsoGridRenderer
{
public:
    IsoGridRenderer();
    ~IsoGridRenderer();

    IsoGridRenderer(const IsoGridRenderer&) = delete;
    IsoGridRenderer& operator=(const IsoGridRenderer&) = delete;

    IsoGridRenderer(IsoGridRenderer&& other) noexcept;
    IsoGridRenderer& operator=(IsoGridRenderer&& other) noexcept;

    void DrawGrid(
        int columns,
        int rows,
        float tileWidth,
        float tileHeight,
        float originX,
        float originY,
        float viewportWidth,
        float viewportHeight
    );

    void DrawTileHighlight(
        int gridX,
        int gridY,
        float tileWidth,
        float tileHeight,
        float originX,
        float originY,
        float viewportWidth,
        float viewportHeight
    );

    void DrawFilledTile(
        int gridX,
        int gridY,
        float tileWidth,
        float tileHeight,
        float originX,
        float originY,
        float viewportWidth,
        float viewportHeight,
        float r,
        float g,
        float b,
        float a
    );

private:
    void EnsureInitialized();
    void Release();

private:
    uint32_t m_ShaderProgram = 0;
    uint32_t m_Vao = 0;
    uint32_t m_Vbo = 0;
};
}
