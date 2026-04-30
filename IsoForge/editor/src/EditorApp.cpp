#include "editor/EditorApp.h"

#include <iostream>

namespace IsoForge
{
EditorApp::EditorApp()
    : Application("IsoForge Editor", 1280, 720)
{
}

void EditorApp::OnInit()
{
    std::cout << "IsoForge Editor initialized." << '\n';
}

void EditorApp::OnShutdown()
{
    std::cout << "IsoForge Editor shutting down." << '\n';
}
}
