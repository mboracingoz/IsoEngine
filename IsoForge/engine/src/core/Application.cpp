#include "engine/core/Application.h"

#include <glad/glad.h>

#include <algorithm>
#include <chrono>

namespace IsoForge
{
Application::Application(const std::string& title, int width, int height)
    : m_window()
    , m_context()
    , m_shouldClose(false)
{
    OpenGLContext::ConfigureAttributes();
    m_window.Create(title, width, height);
    m_context = std::make_unique<OpenGLContext>(m_window.GetNativeWindow());
}

void Application::Run()
{
    using Clock = std::chrono::steady_clock;

    OnInit();

    auto previousTime = Clock::now();
    while (!m_shouldClose)
    {
        const auto currentTime = Clock::now();
        const std::chrono::duration<float> elapsed = currentTime - previousTime;
        previousTime = currentTime;

        const float deltaTime = std::min(elapsed.count(), 0.1f);

        m_window.PollEvents(m_shouldClose);
        OnUpdate(deltaTime);

        glViewport(0, 0, m_window.GetWidth(), m_window.GetHeight());
        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        OnRender();
        m_window.SwapBuffers();
    }

    OnShutdown();
}

void Application::OnInit()
{
}

void Application::OnUpdate(float)
{
}

void Application::OnRender()
{
}

void Application::OnShutdown()
{
}

Window& Application::GetWindow()
{
    return m_window;
}

const Window& Application::GetWindow() const
{
    return m_window;
}
}
