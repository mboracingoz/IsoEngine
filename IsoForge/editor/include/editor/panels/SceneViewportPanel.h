#pragma once

#include "editor/panels/Panel.h"
#include "engine/renderer/Framebuffer.h"

namespace IsoForge
{
class SceneViewportPanel : public Panel
{
public:
    SceneViewportPanel();
    void OnImGuiRender() override;

private:
    Framebuffer m_Framebuffer;
};
}
