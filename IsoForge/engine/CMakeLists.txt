add_library(isoforge_engine INTERFACE)

target_include_directories(isoforge_engine
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(isoforge_engine INTERFACE cxx_std_20)
