#include "engine/platform/OpenGLContext.h"

#include <glad/glad.h>

#include <iostream>
#include <stdexcept>

namespace IsoForge
{
namespace
{
void SetAttribute(SDL_GLAttr attribute, int value)
{
    if (!SDL_GL_SetAttribute(attribute, value))
    {
        throw std::runtime_error(SDL_GetError());
    }
}

}

void OpenGLContext::ConfigureAttributes()
{
    SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

OpenGLContext::OpenGLContext(SDL_Window* window)
    : m_window(window)
    , m_context(nullptr)
{
    m_context = SDL_GL_CreateContext(m_window);
    if (m_context == nullptr)
    {
        throw std::runtime_error(SDL_GetError());
    }

    if (!SDL_GL_MakeCurrent(m_window, m_context))
    {
        SDL_GL_DestroyContext(m_context);
        m_context = nullptr;
        throw std::runtime_error(SDL_GetError());
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        SDL_GL_DestroyContext(m_context);
        m_context = nullptr;
        throw std::runtime_error("Failed to initialize glad.");
    }

    if (!SDL_GL_SetSwapInterval(1))
    {
        std::cerr << "Warning: failed to enable vsync: " << SDL_GetError() << '\n';
    }

    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    const GLubyte* renderer = glGetString(GL_RENDERER);

    std::cout << "OpenGL version: "
              << (version != nullptr ? reinterpret_cast<const char*>(version) : "unknown") << '\n';
    std::cout << "GLSL version: "
              << (glslVersion != nullptr ? reinterpret_cast<const char*>(glslVersion) : "unknown")
              << '\n';
    std::cout << "Renderer: "
              << (renderer != nullptr ? reinterpret_cast<const char*>(renderer) : "unknown")
              << '\n';
}

OpenGLContext::~OpenGLContext()
{
    if (m_context != nullptr)
    {
        SDL_GL_DestroyContext(m_context);
        m_context = nullptr;
    }
}
}
