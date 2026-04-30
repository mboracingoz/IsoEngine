#pragma once

#include <SDL3/SDL.h>

namespace IsoForge
{
class ImGuiLayer
{
public:
    ImGuiLayer() = default;
    ~ImGuiLayer() = default;

    void Init(SDL_Window* window, SDL_GLContext context);
    void Shutdown();

    void BeginFrame();
    void EndFrame();

private:
    bool m_Initialized = false;
};
}
