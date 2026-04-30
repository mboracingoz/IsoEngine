# 13 — Build System

**Project:** IsoForge Editor  
**Version:** 0.2.0  
**Last Updated:** 2025

---

## Overview

IsoForge uses CMake 3.25+ with the Ninja generator. The project is split into three CMake targets:

| Target | Type | Depends on |
|---|---|---|
| `isoforge_engine` | `STATIC` library | SDL3, glad, OpenGL, GLM, EnTT, nlohmann_json, spdlog, stb |
| `isoforge_editor` | Executable | `isoforge_engine`, imgui, ImGuizmo |
| Third-party targets | Various | System libs only |

---

## Prerequisites (CachyOS / Arch Linux)

```bash
# Required
sudo pacman -S cmake ninja gcc git

# OpenGL system library (usually already present with GPU drivers)
sudo pacman -S libgl

# Optional: clang as alternative compiler
sudo pacman -S clang

# Optional: static analysis
sudo pacman -S clang-tools-extra   # clang-tidy, clang-format
```

Verify minimum versions:

```bash
cmake --version    # must be >= 3.25
ninja --version    # must be >= 1.11
gcc --version      # must be >= 12
```

---

## Repository Layout (Build-Relevant Files)

```
IsoForge/
├── CMakeLists.txt             ← Root: sets standards, includes subdirs
├── CMakePresets.json          ← Debug / Release / Sanitizer presets
├── .clang-format              ← Enforced code style
│
├── third_party/
│   ├── CMakeLists.txt         ← FetchContent + vendored lib targets
│   ├── glad/                  ← Pre-generated, committed to repo
│   │   ├── include/
│   │   │   ├── glad/glad.h
│   │   │   └── KHR/khrplatform.h
│   │   └── src/glad.c
│   ├── imgui/                 ← Vendored docking branch
│   ├── ImGuizmo/              ← Vendored
│   └── stb/
│       └── stb_image.h        ← Vendored single header
│
├── engine/
│   └── CMakeLists.txt
│
└── editor/
    └── CMakeLists.txt
```

---

## Root `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.25)
project(IsoForge VERSION 0.1.0 LANGUAGES CXX C)
#                                              ^ C is required for glad.c

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Export compile_commands.json for clangd / IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(ISOFORGE_BUILD_EDITOR  "Build the editor executable" ON)
option(ISOFORGE_ENABLE_ASAN   "Enable AddressSanitizer (Debug only)" OFF)
option(ISOFORGE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

# Warning flags
set(ISOFORGE_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wno-unused-parameter
)
if(ISOFORGE_WARNINGS_AS_ERRORS)
    list(APPEND ISOFORGE_COMPILE_OPTIONS -Werror)
endif()

# Sanitizer
if(ISOFORGE_ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

add_subdirectory(third_party)
add_subdirectory(engine)

if(ISOFORGE_BUILD_EDITOR)
    add_subdirectory(editor)
endif()
```

**Note:** `LANGUAGES CXX C` is required. glad's `glad.c` is a C file and must be compiled as C, not C++.

---

## `third_party/CMakeLists.txt`

```cmake
include(FetchContent)

# ─── Find system OpenGL ───────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)

# ─── glad (vendored, pre-generated for OpenGL 3.3 Core) ──────────────────────
# glad.c must be compiled as C. The root CMakeLists sets LANGUAGES CXX C.
add_library(glad STATIC
    glad/src/glad.c
)
target_include_directories(glad PUBLIC glad/include)
target_link_libraries(glad PUBLIC OpenGL::GL)
# glad.c is C, not C++. Suppress C++ standard flag interference:
set_target_properties(glad PROPERTIES
    C_STANDARD   11
    C_EXTENSIONS OFF
)

# ─── SDL3 (FetchContent) ─────────────────────────────────────────────────────
FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-3.2.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
set(SDL_SHARED  OFF CACHE BOOL "" FORCE)
set(SDL_STATIC  ON  CACHE BOOL "" FORCE)
set(SDL_TEST    OFF CACHE BOOL "" FORCE)
set(SDL_TESTS   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)

# ─── GLM (FetchContent) ───────────────────────────────────────────────────────
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glm)

# ─── EnTT (FetchContent) ──────────────────────────────────────────────────────
FetchContent_Declare(EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.13.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(EnTT)

# ─── nlohmann/json (FetchContent) ────────────────────────────────────────────
FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
set(JSON_BuildTests      OFF CACHE BOOL "" FORCE)
set(JSON_Install         OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(nlohmann_json)

# ─── spdlog (FetchContent) ────────────────────────────────────────────────────
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
    GIT_SHALLOW    TRUE
)
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# ─── stb (vendored single header, INTERFACE — no compile step) ───────────────
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

# ─── Dear ImGui (vendored docking branch) ────────────────────────────────────
add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    imgui
    imgui/backends
)
target_link_libraries(imgui PUBLIC SDL3::SDL3 glad)
# ImGui uses glad for OpenGL — it must NOT link OpenGL::GL directly.
# glad already links OpenGL::GL transitively.

# ─── ImGuizmo (vendored) ──────────────────────────────────────────────────────
add_library(ImGuizmo STATIC
    ImGuizmo/ImGuizmo.cpp
)
target_include_directories(ImGuizmo PUBLIC ImGuizmo)
target_link_libraries(ImGuizmo PUBLIC imgui)
```

---

## `engine/CMakeLists.txt`

```cmake
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(isoforge_engine STATIC ${ENGINE_SOURCES})

target_include_directories(isoforge_engine
    PUBLIC  include          # engine/include — consumed by editor too
    PRIVATE src              # engine/src — internal only
)

target_link_libraries(isoforge_engine
    PUBLIC
        glad                 # OpenGL function loader — PUBLIC so editor gets glad too
        SDL3::SDL3
        glm::glm
        EnTT::EnTT
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        stb
    # imgui is NOT linked here — editor only
)

target_compile_options(isoforge_engine PRIVATE ${ISOFORGE_COMPILE_OPTIONS})
target_compile_features(isoforge_engine PUBLIC cxx_std_20)

# Hard rule: engine must never include ImGui headers.
# This is enforced by not linking imgui here.
# A CI check (grep) can additionally verify no imgui includes in engine/include/.
```

---

## `editor/CMakeLists.txt`

```cmake
file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(isoforge_editor ${EDITOR_SOURCES})

target_include_directories(isoforge_editor
    PRIVATE include          # editor/include
)

target_link_libraries(isoforge_editor
    PRIVATE
        isoforge_engine      # brings glad, SDL3, glm, EnTT, nlohmann, spdlog
        imgui
        ImGuizmo
)

target_compile_options(isoforge_editor PRIVATE ${ISOFORGE_COMPILE_OPTIONS})
```

---

## `CMakePresets.json`

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "debug",
      "displayName": "Debug (Linux, Ninja)",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/debug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE":            "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON",
        "ISOFORGE_ENABLE_ASAN":        "ON"
      }
    },
    {
      "name": "release",
      "displayName": "Release (Linux, Ninja)",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE":            "RelWithDebInfo",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON",
        "ISOFORGE_ENABLE_ASAN":        "OFF"
      }
    },
    {
      "name": "debug-no-asan",
      "displayName": "Debug without ASAN (Linux, Ninja)",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/debug-no-asan",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE":            "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON",
        "ISOFORGE_ENABLE_ASAN":        "OFF"
      }
    }
  ],
  "buildPresets": [
    { "name": "debug",         "configurePreset": "debug"         },
    { "name": "release",       "configurePreset": "release"       },
    { "name": "debug-no-asan", "configurePreset": "debug-no-asan" }
  ]
}
```

---

## Build Commands

### First-time configure and build

```bash
# Configure (downloads FetchContent deps on first run — needs internet)
cmake --preset debug

# Build (uses all available cores via Ninja automatically)
cmake --build build/debug --target isoforge_editor

# Run
./build/debug/bin/isoforge_editor
```

### Rebuild after source changes

```bash
# Ninja only recompiles changed files
cmake --build build/debug --target isoforge_editor
```

### Clean build

```bash
rm -rf build/debug
cmake --preset debug
cmake --build build/debug --target isoforge_editor
```

### Release build

```bash
cmake --preset release
cmake --build build/release --target isoforge_editor
./build/release/bin/isoforge_editor
```

### Symlink compile_commands.json for clangd

```bash
# Run once after first configure
ln -sf build/debug/compile_commands.json compile_commands.json
```

This enables clangd-based autocompletion in VS Code with the `clangd` extension.

---

## glad Setup Procedure

glad files must be committed to the repository. They are pre-generated and do not change unless the OpenGL target version changes.

### How to generate (reference — do not run unless regenerating)

1. Visit https://glad.dav1d.de/
2. Set: Language = C/C++, Specification = OpenGL, API gl = 3.3, Profile = Core
3. Set: Generate a Loader = ✅, Extensions = (none selected)
4. Click Generate, download the ZIP
5. Extract into `third_party/glad/`:
   - `include/glad/glad.h`
   - `include/KHR/khrplatform.h`
   - `src/glad.c`
6. Commit all three files

### glad initialization in application code

```cpp
// In Window.cpp, after SDL_GL_CreateContext():

// Step 1 — SDL3 creates the OpenGL context and makes it current
m_GLContext = SDL_GL_CreateContext(m_Window);
if (!m_GLContext) {
    throw std::runtime_error(
        std::string("SDL_GL_CreateContext failed: ") + SDL_GetError());
}

// Step 2 — glad loads all OpenGL 3.3 Core function pointers
if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
    throw std::runtime_error("gladLoadGLLoader failed: "
        "Could not load OpenGL 3.3 function pointers.");
}

// Step 3 — Verify version (optional but recommended during development)
SDL_Log("OpenGL %s | GLSL %s | %s",
    reinterpret_cast<const char*>(glGetString(GL_VERSION)),
    reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)),
    reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
```

### Include order rule

```cpp
// CORRECT — glad before any other OpenGL header
#include <glad/glad.h>
#include <SDL3/SDL_opengl.h>   // if needed at all (usually not)

// WRONG — never include SDL_opengl.h or gl.h before glad
#include <SDL3/SDL_opengl.h>   // BAD: defines GL types, conflicts with glad
#include <glad/glad.h>
```

In practice, `#include <SDL3/SDL_opengl.h>` is rarely needed in application code. Include `<glad/glad.h>` in any file that calls OpenGL functions, and never include both.

---

## OpenGL 3.3 Core — Permitted vs Banned Functions

All OpenGL call sites must use 3.3 Core API only. The following table shows the correct 3.3 replacements for common 4.5 DSA calls that must not appear in the codebase.

### Framebuffer

```cpp
// ✅ 3.3 Core — CORRECT
GLuint fbo;
glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                        GL_TEXTURE_2D, colorTex, 0);
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                           GL_RENDERBUFFER, depthRbo);
glBindFramebuffer(GL_FRAMEBUFFER, 0);
glDeleteFramebuffers(1, &fbo);

// ❌ 4.5 DSA — BANNED
glCreateFramebuffers(1, &fbo);           // NO DSA
glNamedFramebufferTexture(...);          // NO DSA
glNamedFramebufferRenderbuffer(...);     // NO DSA
```

### Textures

```cpp
// ✅ 3.3 Core — CORRECT
GLuint tex;
glGenTextures(1, &tex);
glBindTexture(GL_TEXTURE_2D, tex);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glBindTexture(GL_TEXTURE_2D, 0);
glDeleteTextures(1, &tex);

// ❌ 4.5 DSA — BANNED
glCreateTextures(GL_TEXTURE_2D, 1, &tex);    // NO DSA
glTextureStorage2D(...);                      // NO DSA
glTextureParameteri(...);                     // NO DSA
```

### Buffers

```cpp
// ✅ 3.3 Core — CORRECT
GLuint vbo;
glGenBuffers(1, &vbo);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glDeleteBuffers(1, &vbo);

// ❌ 4.5 DSA — BANNED
glCreateBuffers(1, &vbo);        // NO DSA
glNamedBufferData(...);          // NO DSA
```

### Vertex Arrays

```cpp
// ✅ 3.3 Core — CORRECT
GLuint vao;
glGenVertexArrays(1, &vao);
glBindVertexArray(vao);
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, offset);
glEnableVertexAttribArray(0);
glBindVertexArray(0);
glDeleteVertexArrays(1, &vao);

// ❌ 4.5 DSA — BANNED
glCreateVertexArrays(1, &vao);            // NO DSA
glVertexArrayAttribFormat(...);           // NO DSA
glEnableVertexArrayAttrib(...);           // NO DSA
```

### Renderbuffers

```cpp
// ✅ 3.3 Core — CORRECT
GLuint rbo;
glGenRenderbuffers(1, &rbo);
glBindRenderbuffer(GL_RENDERBUFFER, rbo);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
glBindRenderbuffer(GL_RENDERBUFFER, 0);
glDeleteRenderbuffers(1, &rbo);

// ❌ 4.5 DSA — BANNED
glCreateRenderbuffers(1, &rbo);                  // NO DSA
glNamedRenderbufferStorage(...);                 // NO DSA
```

---

## Phase 0 — First Implementation Task

**Phase 0 is the actual first task Codex must implement.** It does not include SDL3, OpenGL, or ImGui. It creates only the project skeleton: folder structure, CMakeLists stubs, presets, and formatting config.
In Phase 0, third_party/imgui, third_party/ImGuizmo, and third_party/stb may be created as placeholder folders with .gitkeep files only.

Only glad may be added as real vendored files in Phase 0 if the project wants to validate the OpenGL loader folder contract early.

Actual ImGui, ImGuizmo, and stb source files should be added in the phase where they are first used.
### Phase 0 Goal

The repository compiles (with nothing in it yet) and the folder contract is established. Every subsequent phase adds files into an already-correct structure.
    
### Phase 0: Files to Create

IsoForge/
├── CMakeLists.txt              ← Stub: project(), subdirs only
├── CMakePresets.json           ← debug, release, debug-no-asan
├── .clang-format               ← Formatting rules
├── .gitignore
│
├── third_party/
│   ├── CMakeLists.txt          ← Stub: comment placeholders only
│   ├── glad/                   ← Optional real vendored files in Phase 0
│   │   ├── include/
│   │   │   ├── glad/
│   │   │   │   └── glad.h
│   │   │   └── KHR/
│   │   │       └── khrplatform.h
│   │   └── src/
│   │       └── glad.c
│   ├── imgui/
│   │   └── .gitkeep            ← Placeholder only; real files added in Phase 2
│   ├── ImGuizmo/
│   │   └── .gitkeep            ← Placeholder only; real files added when first used
│   └── stb/
│       └── .gitkeep            ← Placeholder only; stb_image.h added in texture phase
│
├── engine/
│   ├── CMakeLists.txt          ← Phase 0 stub; INTERFACE target only
│   ├── include/
│   │   └── engine/
│   │       └── .gitkeep
│   └── src/
│       └── .gitkeep
│
└── editor/
    ├── CMakeLists.txt          ← Phase 0 stub; builds only main.cpp
    ├── include/
    │   └── editor/
    │       └── .gitkeep
    └── src/
        └── main.cpp            ← Phase 0 stub: int main() { return 0; }
```

### Phase 0: Stub CMakeLists.txt (root)

```cmake
cmake_minimum_required(VERSION 3.25)
project(IsoForge VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(third_party)
add_subdirectory(engine)
add_subdirectory(editor)
```

### Phase 0: Stub `engine/CMakeLists.txt`

```cmake
# Stub — no sources yet. Sources added in Phase 1.
add_library(isoforge_engine STATIC)

target_include_directories(isoforge_engine PUBLIC include)
target_compile_features(isoforge_engine PUBLIC cxx_std_20)
```

Note: CMake requires at least one source file for a STATIC library, or use `INTERFACE` here until Phase 1 adds real sources. Use `INTERFACE` in Phase 0:

```cmake
# Phase 0 only — switch to STATIC in Phase 1 when sources exist
add_library(isoforge_engine INTERFACE)
target_include_directories(isoforge_engine INTERFACE include)
target_compile_features(isoforge_engine INTERFACE cxx_std_20)
```

### Phase 0: Stub `editor/CMakeLists.txt`

```cmake
add_executable(isoforge_editor src/main.cpp)
target_include_directories(isoforge_editor PRIVATE include)
target_link_libraries(isoforge_editor PRIVATE isoforge_engine)
```

### Phase 0: `editor/src/main.cpp`

```cpp
// Phase 0 stub — replaced in Phase 1
int main() {
    return 0;
}
```

### Phase 0: `.clang-format`

```yaml
---
Language: Cpp
BasedOnStyle: LLVM
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100
BreakBeforeBraces: Allman
AllowShortIfStatementsOnASingleLine: false
AllowShortFunctionsOnASingleLine: Inline
AllowShortLoopsOnASingleLine: false
PointerAlignment: Left
ReferenceAlignment: Left
NamespaceIndentation: None
SortIncludes: CaseInsensitive
IncludeBlocks: Regroup
---
```

### Phase 0: `.gitignore`

```
# Build directories
build/

# CMake cache
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile

# Compile commands symlink
compile_commands.json

# Editor directories
.vscode/settings.json
.cache/

# User project data (lives outside repo)
projects/

# Logs
*.log

# Object files
*.o
*.a
*.so

# Binaries
isoforge_editor
```

### Phase 0: Acceptance Criteria

1. `cmake --preset debug` succeeds (no errors, no warnings).
2. `cmake --build build/debug --target isoforge_editor` produces `build/debug/bin/isoforge_editor`.
3. Running `./build/debug/bin/isoforge_editor` exits with code 0.
4. `build/debug/compile_commands.json` exists (for clangd).
5. No source file exists yet in `engine/src/` except `.gitkeep`.
6. The glad headers are present and committed at `third_party/glad/include/glad/glad.h`.

### Phase 0: What NOT to do

- Do not write any engine code yet.
- Do not add SDL3, OpenGL calls, or ImGui in this phase.
- Do not call `gladLoadGLLoader` yet.
- Do not implement any window, renderer, or panel.
- Do not add FetchContent declarations yet (those go in Phase 1 when first needed).

---

## Phase Progression Overview

| Phase | Name | First new dependency introduced |
|---|---|---|
| **0** | Project skeleton | *(none — just CMake structure and vendored files)* |
| 1 | SDL3 window + OpenGL context + glad init | SDL3, glad, OpenGL |
| 2 | Dear ImGui docking layout | imgui |
| 3 | Framebuffer + isometric grid | GLM |
| 4 | Tile hover + coordinate display | *(none)* |
| 5 | Tile atlas + painting + palette panel | stb_image |
| 6 | Tilemap save/load | nlohmann_json |
| 7 | EnTT scene system + hierarchy + inspector | EnTT |
| 8 | Asset browser panel | *(none)* |

Each phase introduces at most one new third-party dependency. This keeps each phase's scope bounded and its build changes reviewable.

---

## Troubleshooting

### `gladLoadGLLoader` returns false

- Cause: Called before `SDL_GL_CreateContext`, or context creation failed silently.
- Fix: Always check `SDL_GL_CreateContext` return value before calling glad.

### Undefined OpenGL symbols at link time

- Cause: `glad` is not linked, or `#include <glad/glad.h>` is missing from the file.
- Fix: Ensure `target_link_libraries(... glad)` is present. `OpenGL::GL` alone is not enough.

### ImGui renders with black textures / invisible UI

- Cause: glad not initialized before `ImGui_ImplOpenGL3_Init`.
- Fix: Call `gladLoadGLLoader` before any ImGui backend init.

### `glCreateFramebuffers` not found / segfault

- Cause: DSA function called on a 3.3 Core context or before glad init.
- Fix: Replace with `glGenFramebuffers` + `glBindFramebuffer`. See OpenGL 3.3 table above.

### FetchContent fails (no network on build server)

- Cause: First build requires internet to clone SDL3, EnTT, etc.
- Fix: Run `cmake --preset debug` once on a machine with internet access. The fetched sources are cached in `build/debug/_deps/` and reused on subsequent builds without network access.

### `LANGUAGES CXX C` not set, glad.c fails to compile

- Cause: Root CMakeLists only declares `LANGUAGES CXX`.
- Fix: Change to `LANGUAGES CXX C`. glad.c is a C source file, not C++.
