#include "editor/EditorApp.h"

#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include <iostream>

namespace IsoForge
{
EditorApp::EditorApp()
    : Application("IsoForge Editor", 1280, 720)
    , m_ImGuiLayer()
    , m_ShowAboutWindow(false)
    , m_ShowDemoWindow(false)
{
}

void EditorApp::OnInit()
{
    m_ImGuiLayer.Init(GetWindow().GetNativeWindow(), GetOpenGLContext().GetNativeContext());
    std::cout << "IsoForge Editor initialized." << '\n';
}

void EditorApp::OnEvent(const SDL_Event& event)
{
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void EditorApp::OnRender()
{
    m_ImGuiLayer.BeginFrame();

    RenderDockspace();

    if (m_ShowAboutWindow)
    {
        RenderAboutWindow();
    }

    if (m_ShowDemoWindow)
    {
        ImGui::ShowDemoWindow(&m_ShowDemoWindow);
    }

    m_ImGuiLayer.EndFrame();
}

void EditorApp::OnShutdown()
{
    m_ImGuiLayer.Shutdown();
    std::cout << "IsoForge Editor shutting down." << '\n';
}

void EditorApp::RenderDockspace()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("IsoForgeDockspaceHost", nullptr, windowFlags);

    ImGui::PopStyleVar(3);

    ImGui::DockSpace(ImGui::GetID("IsoForgeDockspace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                RequestQuit();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("ImGui Demo", nullptr, &m_ShowDemoWindow);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About IsoForge"))
            {
                m_ShowAboutWindow = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void EditorApp::RenderAboutWindow()
{
    if (ImGui::Begin("About IsoForge", &m_ShowAboutWindow, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("IsoForge Editor");
        ImGui::TextUnformatted("Phase 1B -- Dear ImGui Docking Shell");
        ImGui::TextUnformatted("C++20 / SDL3 / OpenGL 3.3 / Dear ImGui");
        ImGui::Separator();

        if (ImGui::Button("Close"))
        {
            m_ShowAboutWindow = false;
        }
    }

    ImGui::End();
}
}
