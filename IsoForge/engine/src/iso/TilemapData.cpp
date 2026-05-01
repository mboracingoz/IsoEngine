#include "engine/iso/TilemapData.h"

#include <algorithm>
#include <ranges>

namespace IsoForge
{
TilemapData::TilemapData(int columns, int rows)
    : m_Columns(std::max(columns, 1))
    , m_Rows(std::max(rows, 1))
    , m_Tiles(static_cast<size_t>(m_Columns * m_Rows), EmptyTile)
{
}

int TilemapData::GetColumns() const
{
    return m_Columns;
}

int TilemapData::GetRows() const
{
    return m_Rows;
}

int TilemapData::GetTileCount() const
{
    return static_cast<int>(m_Tiles.size());
}

int TilemapData::GetFilledTileCount() const
{
    return static_cast<int>(std::ranges::count_if(m_Tiles, [](int tile) {
        return tile != EmptyTile;
    }));
}

bool TilemapData::IsValidCoord(int x, int y) const
{
    return x >= 0 && x < m_Columns && y >= 0 && y < m_Rows;
}

int TilemapData::GetIndex(int x, int y) const
{
    if (!IsValidCoord(x, y))
    {
        return -1;
    }

    return y * m_Columns + x;
}

int TilemapData::GetTile(int x, int y) const
{
    const int index = GetIndex(x, y);
    if (index < 0)
    {
        return EmptyTile;
    }

    return m_Tiles[static_cast<size_t>(index)];
}

const std::vector<int>& TilemapData::GetTiles() const
{
    return m_Tiles;
}

void TilemapData::SetTile(int x, int y, int tileId)
{
    const int index = GetIndex(x, y);
    if (index < 0)
    {
        return;
    }

    m_Tiles[static_cast<size_t>(index)] = tileId;
}

void TilemapData::ClearTile(int x, int y)
{
    SetTile(x, y, EmptyTile);
}

void TilemapData::ClearAll()
{
    std::ranges::fill(m_Tiles, EmptyTile);
}

bool TilemapData::ReplaceTiles(int columns, int rows, const std::vector<int>& tiles)
{
    if (columns <= 0 || rows <= 0)
    {
        return false;
    }

    const size_t expectedTileCount = static_cast<size_t>(columns) * static_cast<size_t>(rows);
    if (tiles.size() != expectedTileCount)
    {
        return false;
    }

    m_Columns = columns;
    m_Rows = rows;
    m_Tiles = tiles;
    return true;
}
}
