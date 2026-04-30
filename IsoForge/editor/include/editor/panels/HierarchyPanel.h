#pragma once

#include "editor/panels/Panel.h"

namespace IsoForge
{
class HierarchyPanel : public Panel
{
public:
    HierarchyPanel();
    void OnImGuiRender() override;
};
}
