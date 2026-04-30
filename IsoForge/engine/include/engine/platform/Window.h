#pragma once

#include <SDL3/SDL.h>

#include <string>

namespace IsoForge
{
class Window
{
public:
    Window();
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void Create(const std::string& title, int width, int height);
    void PollEvents(bool& shouldClose);
    void SwapBuffers() const;

    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;
    [[nodiscard]] SDL_Window* GetNativeWindow() const;

private:
    SDL_Window* m_window;
    int m_width;
    int m_height;
};
}
