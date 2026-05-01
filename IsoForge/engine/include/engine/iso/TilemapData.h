#pragma once

#include <vector>

namespace IsoForge
{
class TilemapData
{
public:
    static constexpr int EmptyTile = -1;

    TilemapData(int columns, int rows);

    [[nodiscard]] int GetColumns() const;
    [[nodiscard]] int GetRows() const;
    [[nodiscard]] int GetTileCount() const;
    [[nodiscard]] int GetFilledTileCount() const;

    [[nodiscard]] bool IsValidCoord(int x, int y) const;
    [[nodiscard]] int GetIndex(int x, int y) const;

    [[nodiscard]] int GetTile(int x, int y) const;
    [[nodiscard]] const std::vector<int>& GetTiles() const;

    void SetTile(int x, int y, int tileId);
    void ClearTile(int x, int y);
    void ClearAll();
    [[nodiscard]] bool ReplaceTiles(int columns, int rows, const std::vector<int>& tiles);

private:
    int m_Columns = 0;
    int m_Rows = 0;
    std::vector<int> m_Tiles;
};
}
