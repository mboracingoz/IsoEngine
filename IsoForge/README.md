# IsoForge Editor

> A focused, editor-based 2D isometric game engine written in C++.

---

## Status

**Early Development — Phase 0: Repository Setup**

This project is in the pre-implementation documentation phase. No engine code has been written yet.

---

## What Is This?

IsoForge Editor is a small-scope, Godot-inspired 2D isometric game engine and editor built from scratch in C++. It is not a general-purpose engine. It is purpose-built for solo indie developers making 2D isometric games on Linux.

It is **not**:
- A Unity or Godot competitor
- A raylib prototype
- A general 2D game framework
- A production-ready engine

It **is**:
- A learning-by-building project
- A practical, editor-first isometric workflow tool
- A tightly scoped C++ engine designed for one developer on modest hardware

---

## Target Platform

- **Primary**: Linux desktop (CachyOS / Arch-based)
- **Hardware**: Low-end friendly (Intel i3-6100, GTX 750 Ti, 8 GB RAM)

---

## Technology Stack

| Layer | Library |
|---|---|
| Language | C++20 |
| Build | CMake |
| Window / Input | SDL3 |
| Rendering | OpenGL 3.3 Core |
| Editor UI | Dear ImGui (Docking branch) |
| Gizmos | ImGuizmo |
| ECS | EnTT |
| Serialization | nlohmann/json |
| Logging | spdlog |
| Image Loading | stb_image |
| Math | glm |
| Audio | miniaudio *(later)* |

---

## Build (Placeholder)

```bash
mkdir -p build
cmake -S . -B build
cmake --build build
./build/IsoForgeEditor
```

> Build instructions will be finalized in Phase 1.

---

## Documentation Index

| File | Description |
|---|---|
| [00_PROJECT_OVERVIEW](docs/00_PROJECT_OVERVIEW.md) | What this project is and isn't |
| [01_REQUIREMENTS](docs/01_REQUIREMENTS.md) | Hardware, software, and non-functional requirements |
| [02_TECH_STACK](docs/02_TECH_STACK.md) | Every library explained and justified |
| [03_ARCHITECTURE](docs/03_ARCHITECTURE.md) | High-level architecture with diagrams |
| [04_FOLDER_STRUCTURE](docs/04_FOLDER_STRUCTURE.md) | Full folder layout with explanations |
| [05_RUNTIME_EDITOR_SEPARATION](docs/05_RUNTIME_EDITOR_SEPARATION.md) | Separation rules and examples |
| [06_RENDERING_AND_VIEWPORT](docs/06_RENDERING_AND_VIEWPORT.md) | Rendering pipeline and framebuffer design |
| [07_ISOMETRIC_SYSTEM](docs/07_ISOMETRIC_SYSTEM.md) | Full isometric coordinate and rendering system |
| [08_SCENE_AND_ECS](docs/08_SCENE_AND_ECS.md) | ECS design using EnTT |
| [09_SERIALIZATION](docs/09_SERIALIZATION.md) | JSON serialization format and strategy |
| [10_ASSET_SYSTEM](docs/10_ASSET_SYSTEM.md) | Texture loading, caching, and browser |
| [11_EDITOR_PANELS](docs/11_EDITOR_PANELS.md) | All editor panel responsibilities |
| [12_LOGGING_AND_CONSOLE](docs/12_LOGGING_AND_CONSOLE.md) | spdlog setup and ImGui console |
| [13_BUILD_SYSTEM](docs/13_BUILD_SYSTEM.md) | CMake structure and build guide |
| [14_PERFORMANCE_GUIDELINES](docs/14_PERFORMANCE_GUIDELINES.md) | Rules for low-end hardware |
| [15_DEVELOPMENT_PHASES](docs/15_DEVELOPMENT_PHASES.md) | Phase-by-phase MVP roadmap |
| [16_CODEX_TASK_RULES](docs/16_CODEX_TASK_RULES.md) | Rules and template for Codex tasks |
| [17_FIRST_IMPLEMENTATION_TASK](docs/17_FIRST_IMPLEMENTATION_TASK.md) | First Codex task: repo setup |

---

## License

*License to be determined. All rights reserved until stated otherwise.*

---

*Solo developer project. Scope is intentionally small. Feature creep is the enemy.*
