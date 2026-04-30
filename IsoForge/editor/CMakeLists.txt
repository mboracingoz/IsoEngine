add_executable(isoforge_editor
    src/main.cpp
    src/EditorApp.cpp
    src/ImGuiLayer.cpp
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
