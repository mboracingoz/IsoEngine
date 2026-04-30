#pragma once

#include "engine/platform/OpenGLContext.h"
#include "engine/platform/Window.h"

#include <memory>
#include <string>

namespace IsoForge
{
class Application
{
public:
    Application(const std::string& title, int width, int height);
    virtual ~Application() = default;

    void Run();

protected:
    virtual void OnInit();
    virtual void OnUpdate(float deltaTime);
    virtual void OnRender();
    virtual void OnShutdown();

    [[nodiscard]] Window& GetWindow();
    [[nodiscard]] const Window& GetWindow() const;

private:
    Window m_window;
    std::unique_ptr<OpenGLContext> m_context;
    bool m_shouldClose;
};
}
