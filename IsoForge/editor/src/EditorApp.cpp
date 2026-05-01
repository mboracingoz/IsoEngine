#include "editor/EditorApp.h"

#include <backends/imgui_impl_sdl3.h>
#include <imgui.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>

namespace IsoForge
{
namespace
{
const std::filesystem::path TilemapDirectory = "sandbox_project/tilemaps";

std::string TrimWhitespace(const std::string& value)
{
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char character) {
        return std::isspace(character) != 0;
    });
    if (first == value.end())
    {
        return {};
    }

    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char character) {
        return std::isspace(character) != 0;
    }).base();

    return std::string(first, last);
}

bool EndsWithJsonExtension(const std::string& fileName)
{
    constexpr size_t JsonExtensionLength = 5;
    if (fileName.size() < JsonExtensionLength)
    {
        return false;
    }

    std::string extension = fileName.substr(fileName.size() - JsonExtensionLength);
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return extension == ".json";
}

bool ContainsInvalidTilemapNameChars(const std::string& fileName)
{
    return fileName.find('/') != std::string::npos ||
           fileName.find('\\') != std::string::npos ||
           fileName.find("..") != std::string::npos;
}

bool TryBuildTilemapPath(std::string rawName, std::filesystem::path& outPath, std::string& outError)
{
    std::string fileName = TrimWhitespace(rawName);
    if (fileName.empty())
    {
        outError = "Tilemap name cannot be empty.";
        return false;
    }

    std::replace(fileName.begin(), fileName.end(), ' ', '_');
    if (ContainsInvalidTilemapNameChars(fileName))
    {
        outError = "Tilemap name cannot contain path separators or '..'.";
        return false;
    }

    if (!EndsWithJsonExtension(fileName))
    {
        fileName += ".json";
    }

    outPath = TilemapDirectory / fileName;
    outError.clear();
    return true;
}

std::vector<std::filesystem::path> CollectTilemapFiles()
{
    std::filesystem::create_directories(TilemapDirectory);

    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(TilemapDirectory))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const std::filesystem::path path = entry.path();
        if (path.extension() == ".json")
        {
            if (path.is_absolute())
            {
                files.push_back(std::filesystem::relative(path, std::filesystem::current_path()));
            }
            else
            {
                files.push_back(path.lexically_normal());
            }
        }
    }

    std::sort(files.begin(), files.end());
    return files;
}
}

EditorApp::EditorApp()
    : Application("IsoForge Editor", 1280, 720)
    , m_EditorState()
    , m_ImGuiLayer()
    , m_SceneViewportPanel(m_EditorState)
    , m_HierarchyPanel()
    , m_InspectorPanel()
    , m_AssetBrowserPanel(m_EditorState)
    , m_TilePalettePanel(m_EditorState)
    , m_ConsolePanel()
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

    m_SceneViewportPanel.OnImGuiRender();
    m_HierarchyPanel.OnImGuiRender();
    m_InspectorPanel.OnImGuiRender();
    m_AssetBrowserPanel.OnImGuiRender();
    m_TilePalettePanel.OnImGuiRender();
    m_ConsolePanel.OnImGuiRender();

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
            if (ImGui::MenuItem("Save Tilemap"))
            {
                if (m_SceneViewportPanel.HasCurrentTilemapPath())
                {
                    const bool saveSucceeded = m_SceneViewportPanel.SaveTilemap();
                    (void)saveSucceeded;
                }
                else
                {
                    m_TilemapPopupMessage.clear();
                    m_OpenSaveTilemapAsPopup = true;
                }
            }

            if (ImGui::MenuItem("Save Tilemap As..."))
            {
                m_TilemapPopupMessage.clear();
                m_OpenSaveTilemapAsPopup = true;
            }

            if (ImGui::MenuItem("Load Tilemap"))
            {
                m_AvailableTilemapFiles = CollectTilemapFiles();
                m_TilemapPopupMessage.clear();
                if (m_AvailableTilemapFiles.empty())
                {
                    m_TilemapPopupMessage = "No tilemaps found.";
                }
                m_OpenLoadTilemapPopup = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit"))
            {
                RequestQuit();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem(
                m_SceneViewportPanel.GetTitle(),
                nullptr,
                &m_SceneViewportPanel.GetOpenRef()
            );
            ImGui::MenuItem(
                m_HierarchyPanel.GetTitle(),
                nullptr,
                &m_HierarchyPanel.GetOpenRef()
            );
            ImGui::MenuItem(
                m_InspectorPanel.GetTitle(),
                nullptr,
                &m_InspectorPanel.GetOpenRef()
            );
            ImGui::MenuItem(
                m_AssetBrowserPanel.GetTitle(),
                nullptr,
                &m_AssetBrowserPanel.GetOpenRef()
            );
            ImGui::MenuItem(
                m_TilePalettePanel.GetTitle(),
                nullptr,
                &m_TilePalettePanel.GetOpenRef()
            );
            ImGui::MenuItem(m_ConsolePanel.GetTitle(), nullptr, &m_ConsolePanel.GetOpenRef());
            ImGui::Separator();
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

    RenderTilemapSaveAsPopup();
    RenderTilemapLoadPopup();

    ImGui::End();
}

void EditorApp::RenderTilemapSaveAsPopup()
{
    if (m_OpenSaveTilemapAsPopup)
    {
        const std::filesystem::path currentPath = m_SceneViewportPanel.GetCurrentTilemapPath();
        if (!currentPath.empty())
        {
            const std::string existingName = currentPath.stem().string();
            std::fill(m_TilemapSaveNameBuffer.begin(), m_TilemapSaveNameBuffer.end(), '\0');
            std::copy_n(
                existingName.c_str(),
                std::min(existingName.size(), m_TilemapSaveNameBuffer.size() - 1),
                m_TilemapSaveNameBuffer.begin()
            );
        }
        else
        {
            std::fill(m_TilemapSaveNameBuffer.begin(), m_TilemapSaveNameBuffer.end(), '\0');
        }

        ImGui::OpenPopup("Save Tilemap As");
        m_OpenSaveTilemapAsPopup = false;
    }

    if (ImGui::BeginPopupModal("Save Tilemap As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Tilemap Directory: sandbox_project/tilemaps/");
        ImGui::InputText("File Name", m_TilemapSaveNameBuffer.data(), m_TilemapSaveNameBuffer.size());

        if (!m_TilemapPopupMessage.empty())
        {
            ImGui::TextWrapped("%s", m_TilemapPopupMessage.c_str());
        }

        if (ImGui::Button("Save"))
        {
            std::filesystem::path path;
            std::string errorMessage;
            if (TryBuildTilemapPath(m_TilemapSaveNameBuffer.data(), path, errorMessage))
            {
                const bool saved = m_SceneViewportPanel.SaveTilemapAs(path);
                if (saved)
                {
                    m_TilemapPopupMessage.clear();
                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    m_TilemapPopupMessage = m_SceneViewportPanel.GetLastTilemapIOStatus();
                }
            }
            else
            {
                m_TilemapPopupMessage = errorMessage;
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            m_TilemapPopupMessage.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void EditorApp::RenderTilemapLoadPopup()
{
    if (m_OpenLoadTilemapPopup)
    {
        ImGui::OpenPopup("Load Tilemap");
        m_OpenLoadTilemapPopup = false;
    }

    if (ImGui::BeginPopupModal("Load Tilemap", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Tilemap Directory: sandbox_project/tilemaps/");

        if (m_AvailableTilemapFiles.empty())
        {
            ImGui::TextUnformatted("No tilemaps found.");
        }
        else
        {
            for (const auto& path : m_AvailableTilemapFiles)
            {
                if (ImGui::Selectable(path.filename().generic_string().c_str()))
                {
                    const bool loaded = m_SceneViewportPanel.LoadTilemap(path);
                    if (loaded)
                    {
                        m_TilemapPopupMessage.clear();
                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        m_TilemapPopupMessage = m_SceneViewportPanel.GetLastTilemapIOStatus();
                    }
                }
            }
        }

        if (!m_TilemapPopupMessage.empty())
        {
            ImGui::Separator();
            ImGui::TextWrapped("%s", m_TilemapPopupMessage.c_str());
        }

        if (ImGui::Button("Close"))
        {
            m_TilemapPopupMessage.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void EditorApp::RenderAboutWindow()
{
    if (ImGui::Begin("About IsoForge", &m_ShowAboutWindow, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("IsoForge Editor");
        ImGui::TextUnformatted("Phase 1C -- Empty Editor Panels");
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
