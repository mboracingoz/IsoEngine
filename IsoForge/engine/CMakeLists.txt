add_library(isoforge_engine STATIC
    src/platform/Window.cpp
    src/platform/OpenGLContext.cpp
    src/core/Application.cpp
)

target_include_directories(isoforge_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(isoforge_engine
    PUBLIC
        SDL3::SDL3
        glad
)

target_compile_features(isoforge_engine PUBLIC cxx_std_20)
