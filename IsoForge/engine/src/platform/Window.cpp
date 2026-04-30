#include "engine/platform/Window.h"

#include <stdexcept>

namespace IsoForge
{
Window::Window()
    : m_window(nullptr)
    , m_width(0)
    , m_height(0)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        throw std::runtime_error(SDL_GetError());
    }
}

void Window::Create(const std::string& title, int width, int height)
{
    m_width = width;
    m_height = height;

    m_window = SDL_CreateWindow(
        title.c_str(),
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (m_window == nullptr)
    {
        const char* error = SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(error);
    }
}

Window::~Window()
{
    if (m_window != nullptr)
    {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    SDL_Quit();
}

void Window::PollEvents(bool& shouldClose)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            shouldClose = true;
        }
        else if (event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            m_width = event.window.data1;
            m_height = event.window.data2;
        }
        else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
        {
            shouldClose = true;
        }
    }
}

void Window::SwapBuffers() const
{
    SDL_GL_SwapWindow(m_window);
}

int Window::GetWidth() const
{
    return m_width;
}

int Window::GetHeight() const
{
    return m_height;
}

SDL_Window* Window::GetNativeWindow() const
{
    return m_window;
}
}
