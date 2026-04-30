#pragma once

#include <SDL3/SDL.h>

namespace IsoForge
{
class OpenGLContext
{
public:
    static void ConfigureAttributes();

    explicit OpenGLContext(SDL_Window* window);
    ~OpenGLContext();

    OpenGLContext(const OpenGLContext&) = delete;
    OpenGLContext& operator=(const OpenGLContext&) = delete;

private:
    SDL_Window* m_window;
    SDL_GLContext m_context;
};
}
