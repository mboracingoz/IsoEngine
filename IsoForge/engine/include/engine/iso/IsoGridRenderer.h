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

    void DrawTexturedTile(
        int gridX,
        int gridY,
        float tileWidth,
        float tileHeight,
        float originX,
        float originY,
        float viewportWidth,
        float viewportHeight,
        uint32_t textureID
    );

private:
    void EnsureInitialized();
    void EnsureTextureInitialized();
    void Release();

private:
    uint32_t m_ColorShaderProgram = 0;
    uint32_t m_ColorVao = 0;
    uint32_t m_ColorVbo = 0;
    uint32_t m_TextureShaderProgram = 0;
    uint32_t m_TextureVao = 0;
    uint32_t m_TextureVbo = 0;
};
}
