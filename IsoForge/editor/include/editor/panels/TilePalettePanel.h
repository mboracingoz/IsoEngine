#pragma once

#include "editor/panels/Panel.h"

namespace IsoForge
{
class TilePalettePanel : public Panel
{
public:
    TilePalettePanel();
    void OnImGuiRender() override;
};
}
