#pragma once

#include "editor/panels/Panel.h"

namespace IsoForge
{
class SceneViewportPanel : public Panel
{
public:
    SceneViewportPanel();
    void OnImGuiRender() override;
};
}
