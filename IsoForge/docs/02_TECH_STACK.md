# 02 — Technology Stack

**Project:** IsoForge Editor  
**Version:** 0.2.0  
**Last Updated:** 2025

---

## Overview

This document defines every library, tool, and standard used in IsoForge Editor. No library may be added to the project without updating this document first.

---

## Language and Standard

| Item | Choice | Reason |
|---|---|---|
| Language | C++20 | Designated initializers, `std::span`, `std::filesystem`, concepts. Modules excluded (poor Linux toolchain support). |
| Standard library | libstdc++ (GCC) / libc++ (Clang) | System-provided on CachyOS. |
| ABI | Itanium (Linux default) | No concern for cross-platform ABI. |

---

## Compiler and Build Tools

| Tool | Version (minimum) | Notes |
|---|---|---|
| GCC | 12+ | Or Clang 15+. Both support C++20 fully. |
| CMake | 3.25+ | Required for `FetchContent` with `GIT_SHALLOW`. |
| Ninja | 1.11+ | Faster than Make on multi-file incremental builds. `pacman -S ninja`. |
| clang-format | 15+ | Enforced by `.clang-format` in repo root. |

---

## Core Dependencies (MVP)

These libraries are required for the first MVP. They are listed in initialization order — a library lower in the table may depend on one above it.

---

### 1. SDL3 — Platform / Window / Input

| Field | Value |
|---|---|
| Role | Window creation, OpenGL context creation, event polling, input |
| Version | `release-3.2.0` (pinned tag) |
| Source | `FetchContent` from `https://github.com/libsdl-org/SDL.git` |
| Link mode | Static (`SDL_STATIC=ON`, `SDL_SHARED=OFF`) |
| CMake target | `SDL3::SDL3` |
| Header | `<SDL3/SDL.h>` |
| Used by | `engine` (Window, Application), `editor` (ImGui backend) |

**Why SDL3 and not SDL2:** SDL3 has a cleaner API, better Wayland support on modern Linux, and first-class CMake integration. The CachyOS target benefits from its Wayland improvements.

**Critical usage note:** SDL3 creates the OpenGL context but does **not** load OpenGL function pointers. glad (see below) must be initialized immediately after SDL3 creates the context.

---

### 2. glad — OpenGL Function Loader

| Field | Value |
|---|---|
| Role | Load OpenGL 3.3 Core Profile function pointers at runtime |
| Version | Generated for OpenGL 3.3 Core, no extensions |
| Source | **Vendored** — `third_party/glad/` (pre-generated source included in repo) |
| Link mode | Compiled into `isoforge_engine` as a static object |
| CMake target | `glad` (interface library defined in `third_party/CMakeLists.txt`) |
| Headers | `<glad/glad.h>` |
| Used by | `engine` only — no editor code calls OpenGL directly |

**Why glad is required:**

On Linux, OpenGL function pointers beyond the base OpenGL 1.1 ABI are not available by linking `-lGL` alone. Functions such as `glGenFramebuffers`, `glUseProgram`, `glGenVertexArrays`, and all of modern OpenGL must be retrieved via `glXGetProcAddress` (GLX) or the platform equivalent. glad generates a single `glad.c` file that performs all of these lookups at startup using the platform's procedure address function.

**Initialization order is mandatory:**

```cpp
// CORRECT order — always:
SDL_GL_CreateContext(window);   // 1. SDL3 creates context (makes it current)
gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);  // 2. glad loads pointers
// Only now is it safe to call any OpenGL function.
```

Calling any OpenGL function before `gladLoadGLLoader` will result in a null function pointer dereference and a segfault.

**Generation parameters (for reference — pre-generated files are committed):**

```
Language:  C/C++
Specification: gl
APIs: gl=3.3
Profile: core
Extensions: (none)
Options: Generate a loader
```

The pre-generated files committed to the repository are:
- `third_party/glad/include/glad/glad.h`
- `third_party/glad/include/KHR/khrplatform.h`
- `third_party/glad/src/glad.c`

**OpenGL version policy — 3.3 Core only:**

IsoForge targets OpenGL 3.3 Core Profile. The GTX 750 Ti supports OpenGL 4.5 but we deliberately restrict to 3.3 for maximum compatibility and driver stability on Linux.

The following OpenGL 4.x Direct State Access (DSA) functions are **banned** from the codebase:

| Banned (4.5 DSA) | Use instead (3.3) |
|---|---|
| `glCreateFramebuffers` | `glGenFramebuffers` + `glBindFramebuffer` |
| `glCreateTextures` | `glGenTextures` + `glBindTexture` |
| `glCreateBuffers` | `glGenBuffers` + `glBindBuffer` |
| `glCreateVertexArrays` | `glGenVertexArrays` + `glBindVertexArray` |
| `glCreateRenderbuffers` | `glGenRenderbuffers` + `glBindRenderbuffer` |
| `glNamedBufferData` | `glBufferData` (after bind) |
| `glTextureParameteri` | `glTexParameteri` (after bind) |

A `// NO DSA` comment in code review marks any accidental DSA usage for immediate revert.

---

### 3. OpenGL 3.3 Core Profile — Rendering API

| Field | Value |
|---|---|
| Role | GPU rendering |
| Version | 3.3 Core Profile |
| Source | System libGL (loaded via glad at runtime) |
| Link | `-lGL` (via CMake `find_package(OpenGL REQUIRED)`) |
| CMake target | `OpenGL::GL` |
| Used by | `engine/renderer` only |

---

### 4. Dear ImGui (Docking branch) — Editor UI

| Field | Value |
|---|---|
| Role | All editor UI: panels, dockspace, widgets |
| Version | Docking branch, commit pinned in `third_party/imgui/` |
| Source | **Vendored** — `third_party/imgui/` |
| Backends used | `imgui_impl_sdl3.cpp`, `imgui_impl_opengl3.cpp` |
| CMake target | `imgui` |
| Headers | `<imgui.h>`, `<imgui_impl_sdl3.h>`, `<imgui_impl_opengl3.h>` |
| Used by | `editor` only — **never included in `engine/` headers** |

**Why vendored:** The docking branch is not a release. It is a long-lived development branch that must be manually pinned. The `imgui_impl_*` backend files are expected to be modified (e.g. to handle SDL3 event forwarding details). FetchContent is unsuitable for non-release branches that require source-level editing.

---

### 5. ImGuizmo — Viewport Gizmos

| Field | Value |
|---|---|
| Role | In-viewport transform gizmos (translate, rotate, scale handles) |
| Version | Latest commit, vendored |
| Source | **Vendored** — `third_party/ImGuizmo/` |
| CMake target | `ImGuizmo` |
| Used by | `editor` only |

**MVP status:** ImGuizmo is included in the dependency list but **not used until Phase 6** (entity transform editing). It is vendored now so the CMake graph is complete.

---

### 6. EnTT — Entity Component System

| Field | Value |
|---|---|
| Role | Entity management, component storage, system iteration |
| Version | `v3.13.2` (pinned tag) |
| Source | `FetchContent` |
| Link mode | Header-only |
| CMake target | `EnTT::EnTT` |
| Headers | `<entt/entt.hpp>` |
| Used by | `engine/scene` only |

---

### 7. GLM — Math Library

| Field | Value |
|---|---|
| Role | Vectors, matrices, quaternions, projections |
| Version | `1.0.1` (pinned tag) |
| Source | `FetchContent` |
| Link mode | Header-only |
| CMake target | `glm::glm` |
| Headers | `<glm/glm.hpp>`, `<glm/gtc/matrix_transform.hpp>`, etc. |
| Used by | `engine`, `editor` |

**Convention:** Always use `glm::vec2`, `glm::vec3`, `glm::mat4`. Never use raw float arrays for math.

---

### 8. nlohmann/json — Serialization

| Field | Value |
|---|---|
| Role | Scene, tilemap, and project JSON read/write |
| Version | `v3.11.3` (pinned tag) |
| Source | `FetchContent` |
| Link mode | Header-only (single-include) |
| CMake target | `nlohmann_json::nlohmann_json` |
| Headers | `<nlohmann/json.hpp>` |
| Used by | `engine/serialization`, `editor/project` |

---

### 9. spdlog — Logging

| Field | Value |
|---|---|
| Role | Structured, leveled logging for engine and editor |
| Version | `v1.14.1` (pinned tag) |
| Source | `FetchContent` |
| Link mode | Header-only (default) or compiled (`SPDLOG_COMPILED_LIB`) |
| CMake target | `spdlog::spdlog` |
| Headers | `<spdlog/spdlog.h>`, `<spdlog/sinks/stdout_color_sinks.h>`, etc. |
| Used by | `engine/core/Log`, `editor/panels/ConsolePanel` |

---

### 10. stb_image — Image Loading

| Field | Value |
|---|---|
| Role | Load PNG, JPG, BMP textures from disk |
| Version | Latest (vendored single header) |
| Source | **Vendored** — `third_party/stb/stb_image.h` |
| CMake target | `stb` (INTERFACE library) |
| Usage | `#define STB_IMAGE_IMPLEMENTATION` in exactly one `.cpp` file |
| Used by | `engine/assets/AssetManager.cpp` only |

**Rule:** `STB_IMAGE_IMPLEMENTATION` is defined in `engine/src/assets/AssetManager.cpp` and nowhere else. Including `stb_image.h` without the implementation define in other files is safe.

---

## Libraries Explicitly Excluded from MVP

| Library | Reason for exclusion |
|---|---|
| Vulkan | Complexity far exceeds MVP needs. GTX 750 Ti supports it but driver overhead is not justified. |
| Box2D | Physics not in MVP scope. |
| sol2 / Lua | Scripting not in MVP scope. |
| Tracy Profiler | Profiling not needed until Phase 5+. |
| miniaudio | Audio not in MVP scope. |
| GLFW | SDL3 already provides window and input. Two window libraries is redundant. |
| GLEW | glad is lighter, generates exactly the API surface needed, and has no runtime `.dll` dependency. |

---

## Hardware Compatibility Matrix

| GPU | OpenGL | Driver | Status |
|---|---|---|---|
| NVIDIA GTX 750 Ti | 4.5 (using 3.3) | proprietary / nouveau | ✅ Primary dev target |
| Intel HD 4000+ | 4.0 (using 3.3) | Mesa | ✅ Compatible |
| AMD Radeon HD 7000+ | 4.1 (using 3.3) | AMDGPU / Mesa | ✅ Compatible |
| Any GPU with Mesa GL 3.3+ | 3.3 | Mesa | ✅ Compatible |

---

## Dependency Acquisition Summary

| Library | Method | Pinned |
|---|---|---|
| SDL3 | FetchContent | ✅ `release-3.2.0` |
| glad | Vendored (pre-generated) | ✅ GL 3.3 Core |
| OpenGL | System (`find_package`) | N/A |
| Dear ImGui | Vendored | ✅ commit hash |
| ImGuizmo | Vendored | ✅ commit hash |
| stb_image | Vendored | ✅ file hash |
| EnTT | FetchContent | ✅ `v3.13.2` |
| GLM | FetchContent | ✅ `1.0.1` |
| nlohmann/json | FetchContent | ✅ `v3.11.3` |
| spdlog | FetchContent | ✅ `v1.14.1` |
