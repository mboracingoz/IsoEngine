#pragma once

#include "editor/panels/Panel.h"

namespace IsoForge
{
class ConsolePanel : public Panel
{
public:
    ConsolePanel();
    void OnImGuiRender() override;
};
}
