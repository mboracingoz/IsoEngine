# Documentation Index

This directory contains all design, architecture, implementation planning, workflow, and Codex task documentation for IsoForge Editor.

---

## Document List

| # | File | Topic / Purpose |
|---|---|---|
| 00 | [00_PROJECT_OVERVIEW.md](00_PROJECT_OVERVIEW.md) | Project vision, goals, and high-level scope |
| 01 | [01_REQUIREMENTS.md](01_REQUIREMENTS.md) | Hardware, software, and platform requirements |
| 02 | [02_TECH_STACK.md](02_TECH_STACK.md) | Library choices, version decisions, and rejected technologies |
| 03 | [03_ARCHITECTURE.md](03_ARCHITECTURE.md) | Module layout, layer boundaries, and dependency rules |
| 04 | [04_FOLDER_STRUCTURE.md](04_FOLDER_STRUCTURE.md) | Repository folder layout and file placement conventions |
| 05 | [05_RUNTIME_EDITOR_SEPARATION.md](05_RUNTIME_EDITOR_SEPARATION.md) | Hard rules for keeping editor and runtime code separate |
| 06 | [06_RENDERING_AND_VIEWPORT.md](06_RENDERING_AND_VIEWPORT.md) | Framebuffer, Renderer2D, OpenGL context lifecycle |
| 07 | [07_ISOMETRIC_SYSTEM.md](07_ISOMETRIC_SYSTEM.md) | Isometric coordinate math, IsoPicker, and grid rendering |
| 08 | [08_SCENE_AND_ECS.md](08_SCENE_AND_ECS.md) | EnTT scene, entity and component lifecycle |
| 09 | [09_SERIALIZATION.md](09_SERIALIZATION.md) | Scene and tilemap JSON format, SceneSerializer |
| 10 | [10_ASSET_SYSTEM.md](10_ASSET_SYSTEM.md) | Asset loading pipeline, texture cache, and AssetManager |
| 11 | [11_EDITOR_PANELS.md](11_EDITOR_PANELS.md) | Panel responsibilities, context objects, and communication rules |
| 12 | [12_LOGGING_AND_CONSOLE.md](12_LOGGING_AND_CONSOLE.md) | Logging architecture, ConsoleLogSink, and ConsolePanel |
| 13 | [13_BUILD_SYSTEM.md](13_BUILD_SYSTEM.md) | CMake structure, targets, presets, and build instructions |
| 14 | [14_PERFORMANCE_GUIDELINES.md](14_PERFORMANCE_GUIDELINES.md) | Low-end hardware rules, batch limits, and frame budget |
| 15 | [15_DEVELOPMENT_PHASES.md](15_DEVELOPMENT_PHASES.md) | MVP roadmap — phased feature plan from Phase 0 to Phase 15 |
| 16 | [16_CODEX_TASK_RULES.md](16_CODEX_TASK_RULES.md) | Rules Codex must follow when executing implementation tasks |
| 17 | [17_FIRST_IMPLEMENTATION_TASK.md](17_FIRST_IMPLEMENTATION_TASK.md) | Step-by-step instructions for the first concrete implementation task |
| 18 | [18_WORKFLOW_DIAGRAMS.md](18_WORKFLOW_DIAGRAMS.md) | Mermaid working diagrams — editor lifecycle, frame loop, input routing, tile painting, save/load, dependency rules, and coordinate conversion |

---

## Recommended Reading Order

Read the following files in order before writing any code or starting any task:

1. [00_PROJECT_OVERVIEW.md](00_PROJECT_OVERVIEW.md)
2. [01_REQUIREMENTS.md](01_REQUIREMENTS.md)
3. [02_TECH_STACK.md](02_TECH_STACK.md)
4. [03_ARCHITECTURE.md](03_ARCHITECTURE.md)
5. [04_FOLDER_STRUCTURE.md](04_FOLDER_STRUCTURE.md)
6. [05_RUNTIME_EDITOR_SEPARATION.md](05_RUNTIME_EDITOR_SEPARATION.md)
7. [15_DEVELOPMENT_PHASES.md](15_DEVELOPMENT_PHASES.md)
8. [16_CODEX_TASK_RULES.md](16_CODEX_TASK_RULES.md)
9. [17_FIRST_IMPLEMENTATION_TASK.md](17_FIRST_IMPLEMENTATION_TASK.md)
10. [18_WORKFLOW_DIAGRAMS.md](18_WORKFLOW_DIAGRAMS.md)

The remaining files (06–14) should be read when actively working on their specific system. For example, read `07_ISOMETRIC_SYSTEM.md` when implementing tile picking, and `09_SERIALIZATION.md` when implementing save/load.

---

## Quick Navigation

| Goal | Document |
|---|---|
| Project scope and vision | [00_PROJECT_OVERVIEW.md](00_PROJECT_OVERVIEW.md) |
| Hardware / software requirements | [01_REQUIREMENTS.md](01_REQUIREMENTS.md) |
| Library decisions and rejected technologies | [02_TECH_STACK.md](02_TECH_STACK.md) |
| Architecture and dependency rules | [03_ARCHITECTURE.md](03_ARCHITECTURE.md) |
| Folder layout | [04_FOLDER_STRUCTURE.md](04_FOLDER_STRUCTURE.md) |
| Runtime / editor boundary | [05_RUNTIME_EDITOR_SEPARATION.md](05_RUNTIME_EDITOR_SEPARATION.md) |
| Rendering and framebuffer viewport | [06_RENDERING_AND_VIEWPORT.md](06_RENDERING_AND_VIEWPORT.md) |
| Isometric math and tile picking | [07_ISOMETRIC_SYSTEM.md](07_ISOMETRIC_SYSTEM.md) |
| ECS and scene system | [08_SCENE_AND_ECS.md](08_SCENE_AND_ECS.md) |
| JSON save / load | [09_SERIALIZATION.md](09_SERIALIZATION.md) |
| Asset loading and texture cache | [10_ASSET_SYSTEM.md](10_ASSET_SYSTEM.md) |
| Editor panels | [11_EDITOR_PANELS.md](11_EDITOR_PANELS.md) |
| Logging and console panel | [12_LOGGING_AND_CONSOLE.md](12_LOGGING_AND_CONSOLE.md) |
| Build system and CMake | [13_BUILD_SYSTEM.md](13_BUILD_SYSTEM.md) |
| Low-end hardware performance rules | [14_PERFORMANCE_GUIDELINES.md](14_PERFORMANCE_GUIDELINES.md) |
| Development phases | [15_DEVELOPMENT_PHASES.md](15_DEVELOPMENT_PHASES.md) |
| Codex implementation rules | [16_CODEX_TASK_RULES.md](16_CODEX_TASK_RULES.md) |
| First implementation task | [17_FIRST_IMPLEMENTATION_TASK.md](17_FIRST_IMPLEMENTATION_TASK.md) |
| Working diagrams and execution flow | [18_WORKFLOW_DIAGRAMS.md](18_WORKFLOW_DIAGRAMS.md) |

---

## Codex Usage Rule

Before Codex implements any task, it must read:

- [15_DEVELOPMENT_PHASES.md](15_DEVELOPMENT_PHASES.md) — to confirm the task is within the current active phase
- [16_CODEX_TASK_RULES.md](16_CODEX_TASK_RULES.md) — to follow the correct task execution process
- The specific system document related to the task (e.g. `07_ISOMETRIC_SYSTEM.md` for tile picking)
- [18_WORKFLOW_DIAGRAMS.md](18_WORKFLOW_DIAGRAMS.md) — if the task touches execution flow, rendering, input, tile painting, save/load, or dependencies

---

## Do Not Skip

Codex must never skip the following, regardless of task size or apparent simplicity:

- **Runtime / editor separation rules** — engine code must never include editor headers; see `05_RUNTIME_EDITOR_SEPARATION.md` and diagram 6 in `18_WORKFLOW_DIAGRAMS.md`
- **Current phase boundaries** — do not implement features belonging to a future phase; see `15_DEVELOPMENT_PHASES.md` and diagram 9 in `18_WORKFLOW_DIAGRAMS.md`
- **Dependency direction rules** — editor may depend on engine, engine must not depend on editor; see `03_ARCHITECTURE.md` and diagram 7 in `18_WORKFLOW_DIAGRAMS.md`
- **Manual test steps** — every task must be built and manually verified before reporting completion; see `16_CODEX_TASK_RULES.md`
- **Documentation updates** — if a workflow or architecture changes, the corresponding diagram(s) in `18_WORKFLOW_DIAGRAMS.md` must be updated in the same commit
