include(FetchContent)

find_package(OpenGL REQUIRED)

add_library(glad STATIC
    glad/src/glad.c
)

target_include_directories(glad
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
)

target_link_libraries(glad
    PUBLIC
        OpenGL::GL
)

set_target_properties(glad PROPERTIES
    C_STANDARD 11
    C_EXTENSIONS OFF
)

FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_VIDEO ON CACHE BOOL "" FORCE)
set(SDL_OPENGL ON CACHE BOOL "" FORCE)
set(SDL_PIPEWIRE OFF CACHE BOOL "" FORCE)
set(SDL_PULSEAUDIO OFF CACHE BOOL "" FORCE)
set(SDL_ALSA OFF CACHE BOOL "" FORCE)
set(SDL_JACK OFF CACHE BOOL "" FORCE)
set(SDL_ESD OFF CACHE BOOL "" FORCE)
set(SDL_SNDIO OFF CACHE BOOL "" FORCE)
set(SDL_DISKAUDIO OFF CACHE BOOL "" FORCE)
set(SDL_DUMMYAUDIO OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(nlohmann_json)

add_library(stb INTERFACE)

target_include_directories(stb
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/stb
)

add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

target_link_libraries(imgui
    PUBLIC
        SDL3::SDL3
        glad
)
