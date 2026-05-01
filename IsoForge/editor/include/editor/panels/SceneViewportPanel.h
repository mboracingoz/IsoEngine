#pragma once

#include "editor/EditorCamera.h"
#include "editor/panels/Panel.h"
#include "engine/iso/IsoGridRenderer.h"
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
    IsoGridRenderer m_IsoGridRenderer;
};
}
