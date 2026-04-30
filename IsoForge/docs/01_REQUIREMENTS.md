# 01 — Requirements

---

## Hardware Requirements

### Development Machine (Reference System)

| Component | Specification |
|---|---|
| CPU | Intel Core i3-6100 (2 cores / 4 threads, 3.7 GHz) |
| GPU | NVIDIA GTX 750 Ti (2 GB VRAM, OpenGL 4.5 capable) |
| RAM | 8 GB DDR4 |
| Storage | 250 GB SSD (primary) + 1 TB external HDD |

### Minimum Runtime Target (Editor)

The editor must run acceptably on hardware at or near the development machine specifications above. There is no dedicated "minimum spec" beyond the development hardware.

The architecture must not assume more than:
- 4 GB RAM available to the application during editing.
- 512 MB VRAM usage by the renderer and editor UI combined in an average session.
- A single-core-equivalent performance budget for the editor's per-frame work.

---

## Software Requirements

### Operating System

- **Primary**: CachyOS Linux (Arch-based)
- **Linux assumption**: Any modern Arch-based or Debian-based Linux with X11 or Wayland (via SDL3 abstraction) should work.
- **Windows/macOS**: Not a target in the MVP. Not excluded in the long term, but not documented here.

### Shell

- **fish shell** is the developer's preferred shell.
- Build scripts and instructions must be compatible with both fish and bash.
- Do not write shell scripts using bash-only syntax if they will be run interactively in fish.

### Compiler

- **GCC 13+** or **Clang 17+** with C++20 support.
- The project must compile cleanly with `-std=c++20`.
- Warnings should be enabled: `-Wall -Wextra`.
- Warnings are not required to be errors in MVP, but the codebase should aim for clean output.

### Build System

- **CMake 3.25+**
- No other build system is required or supported.

### IDE / Editor

- **VS Code** with the following extensions:
  - C/C++ (Microsoft) or **clangd** (preferred)
  - CMake Tools
  - CMake language support

### GPU Driver

- NVIDIA proprietary driver or Mesa (for Intel fallback).
- OpenGL 3.3 Core Profile must be available.

### OpenGL

- **OpenGL 3.3 Core Profile** is the rendering API.
- No extensions beyond what 3.3 Core provides are required in the MVP.
- No Vulkan. No OpenGL ES.

---

## Development Environment

```
OS:       CachyOS Linux (Arch-based)
Shell:    fish
IDE:      VS Code
Compiler: GCC 13+ or Clang 17+
Build:    CMake 3.25+
Debugger: GDB or LLDB
```

Required system packages (Arch/CachyOS):

```bash
sudo pacman -S base-devel cmake gcc clang git mesa libgl
```

SDL3, Dear ImGui, and other libraries are managed as vendored source in `third_party/` or via CMake FetchContent. See `13_BUILD_SYSTEM.md` for details.

---

## Target Platform

- **Linux desktop** — X11 or Wayland via SDL3.
- The editor window is a native desktop window created by SDL3.
- No browser target. No mobile target. No embedded target.

---

## Non-Functional Requirements

These are constraints that govern the entire codebase and architecture. They are as important as the functional requirements.

### 1. Low Memory Usage

- The editor must not exceed ~500 MB RAM during typical editing sessions on a 2D isometric project.
- Textures must be loaded on demand, not all at startup.
- Avoid loading large asset collections into memory without explicit user action.
- Prefer stack allocation and RAII over heap allocation where practical.

### 2. Short Build Iterations

- Incremental builds after small code changes must complete in under 30 seconds on the reference hardware.
- Avoid header-only mega-libraries that trigger full rebuilds on every change.
- Use precompiled headers (PCH) if build times become problematic in later phases.
- Separate engine and editor into distinct CMake targets to allow incremental compilation.

### 3. Clean Module Separation

- Each major system (renderer, scene, asset, serialization, input) must be a coherent, self-contained module.
- Modules must have clearly defined interfaces.
- No "god class" that owns and drives everything.
- No circular dependencies between modules.

### 4. Editor / Runtime Separation

- Code in `engine/` must never `#include` headers from `editor/`.
- The engine must be usable without the editor in the future.
- Editor-only systems (panels, commands, selection state) must live entirely in `editor/`.
- See `05_RUNTIME_EDITOR_SEPARATION.md` for full rules.

### 5. Maintainability

- Files should be small and focused. Prefer many small files over few large files.
- Functions should do one thing.
- Class responsibilities should be clear and minimal.
- Every file must have a clear owner (engine, editor, or project data).

### 6. Testability

- Engine modules should be testable without the editor running.
- Prefer pure functions for coordinate math, serialization helpers, and utility code.
- Unit tests are not required in the MVP but the architecture must not prevent them.

### 7. Low-End GPU Compatibility

- Only OpenGL 3.3 Core features are allowed.
- No compute shaders.
- No geometry shaders (in MVP).
- No bindless textures.
- No multi-draw indirect.
- Keep draw calls simple. Optimize only after correctness is confirmed.

### 8. Solo Developer Scope Control

- Every feature must justify its cost in implementation time.
- "Later, not MVP" is a valid and preferred answer for any feature not required to ship the MVP phases.
- The roadmap in `15_DEVELOPMENT_PHASES.md` defines the boundary.
- No feature may be added to the MVP without updating the roadmap document.
