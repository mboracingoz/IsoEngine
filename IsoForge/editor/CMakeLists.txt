add_executable(isoforge_editor
    src/main.cpp
    src/EditorApp.cpp
    src/EditorCamera.cpp
    src/ImGuiLayer.cpp
    src/panels/SceneViewportPanel.cpp
    src/panels/HierarchyPanel.cpp
    src/panels/InspectorPanel.cpp
    src/panels/AssetBrowserPanel.cpp
    src/panels/TilePalettePanel.cpp
    src/panels/ConsolePanel.cpp
)

target_include_directories(isoforge_editor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(isoforge_editor
    PRIVATE
        isoforge_engine
        imgui
)
