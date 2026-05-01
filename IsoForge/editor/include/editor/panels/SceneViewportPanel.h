#pragma once

#include "editor/EditorCamera.h"
#include "editor/panels/Panel.h"
#include "engine/iso/IsoMath.h"
#include "engine/iso/IsoGridRenderer.h"
#include "engine/iso/TilemapData.h"
#include "engine/renderer/Framebuffer.h"

namespace IsoForge
{
class SceneViewportPanel : public Panel
{
public:
    SceneViewportPanel();
    void OnImGuiRender() override;

private:
    EditorCamera m_EditorCamera;
    Framebuffer m_Framebuffer;
    TilemapData m_Tilemap {20, 20};
    HoverResult m_HoverResult;
    IsoGridRenderer m_IsoGridRenderer;
};
}
