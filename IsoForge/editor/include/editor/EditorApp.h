#pragma once

#include "editor/ImGuiLayer.h"
#include "engine/core/Application.h"

namespace IsoForge
{
class EditorApp : public Application
{
public:
    EditorApp();
    ~EditorApp() override = default;

protected:
    void OnInit() override;
    void OnEvent(const SDL_Event& event) override;
    void OnRender() override;
    void OnShutdown() override;

private:
    void RenderDockspace();
    void RenderAboutWindow();

    ImGuiLayer m_ImGuiLayer;
    bool m_ShowAboutWindow;
    bool m_ShowDemoWindow;
};
}
