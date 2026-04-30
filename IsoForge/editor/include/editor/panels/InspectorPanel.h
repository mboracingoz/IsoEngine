#pragma once

#include "editor/panels/Panel.h"

namespace IsoForge
{
class InspectorPanel : public Panel
{
public:
    InspectorPanel();
    void OnImGuiRender() override;
};
}
