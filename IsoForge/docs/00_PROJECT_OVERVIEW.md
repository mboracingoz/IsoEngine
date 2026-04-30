# 00 — Project Overview

## Project Name

**IsoForge Editor**

---

## Short Description

IsoForge Editor is a focused, editor-based 2D isometric game engine written in C++20. It is designed for solo indie developers who want a lightweight, custom toolchain for making 2D isometric games on Linux desktop.

---

## What This Project Is

- A **native desktop editor application** with a scene viewport, hierarchy panel, inspector panel, tile palette, and asset browser.
- A **2D isometric tilemap editor** with tile painting, erasing, and JSON-based save/load.
- A **minimal entity-component system** (ECS) powered by EnTT for scene management.
- An **editor-first workflow tool**: the editor is the primary interface, not a runtime player.
- A **learning and production project** for one developer building real isometric games.
- A **low-end hardware friendly** codebase that runs well on modest Linux hardware.
- Inspired in spirit by **Godot**, but much smaller, more focused, and built from scratch.

---

## What This Project Is Not

- **Not a Unity or Godot competitor.** It does not aim to replace general-purpose engines.
- **Not a raylib prototype.** It is a full editor application with dockable panels, not a code-first game framework.
- **Not a game framework.** It is a game editor that contains a runtime engine as one of its layers.
- **Not a Vulkan project.** OpenGL 3.3 Core is used intentionally for simplicity and compatibility.
- **Not a scripting engine.** Lua/scripting is a future possibility, not an MVP feature.
- **Not a physics engine.** Physics is a future possibility, not an MVP feature.
- **Not production-ready for other developers.** This is a personal solo project.

---

## Target User

A solo indie developer who:

- Is comfortable with C++ and wants to build their own tooling.
- Wants to make 2D isometric games without learning a massive engine.
- Is working on Linux desktop with modest hardware.
- Values understanding every part of the system they build.
- Prefers focused tools over bloated general-purpose solutions.

---

## Target Game Type

**2D isometric games**, including:

- Isometric RPGs
- Isometric strategy games
- Isometric city builders
- Isometric dungeon crawlers

The editor is purpose-built for isometric workflows. It is not intended for top-down, side-scrolling, or 3D games.

---

## Long-Term Vision

Over time, IsoForge Editor aims to support:

- A native dockable editor layout with persistent settings.
- A full isometric tilemap editor with layers and depth sorting.
- An entity-component scene system with inspector and hierarchy panels.
- A basic asset browser and texture management system.
- Scene save/load with JSON serialization.
- Play/stop mode for in-editor game testing.
- Undo/redo history.
- Editor gizmos for entity transforms.
- An asset import pipeline for textures and tilemaps.
- Project system with project templates.
- Optional: audio with miniaudio.
- Optional: physics with Box2D.
- Optional: scripting with Lua and sol2.
- Optional: performance profiling with Tracy Profiler.

None of these long-term features are required in the MVP.

---

## MVP Vision

The MVP is a minimal but functional isometric editor that proves the architecture works.

The MVP must deliver:

1. A native SDL3 window with an OpenGL 3.3 context.
2. Dear ImGui Docking layout with basic editor panels.
3. A scene viewport panel rendering an OpenGL framebuffer inside ImGui.
4. A basic isometric grid rendered inside that viewport.
5. Camera pan and zoom inside the viewport.
6. Mouse hover tile detection on the isometric grid.
7. A console/log panel with basic log output.
8. A tile palette panel with selectable tiles.
9. Tile painting and erasing on a tilemap.
10. Tilemap save and load as JSON.
11. An EnTT-based scene system with basic entity management.
12. Hierarchy panel showing the entity tree.
13. Inspector panel showing entity components.
14. Basic sprite rendering.
15. Scene save and load as JSON.

The MVP does **not** include: physics, scripting, audio, animation, advanced lighting, complex asset pipeline, or runtime export.

---

## Current Development Constraints

- **Hardware**: Intel i3-6100 CPU, GTX 750 Ti GPU, 8 GB RAM, 250 GB SSD.
- **OS**: CachyOS Linux (Arch-based). Linux-first. No Windows or macOS support in MVP.
- **IDE**: VS Code with Clangd and CMake Tools.
- **Developer**: Solo. No team. No CI/CD required in MVP.
- **Build time**: Must stay short. Incremental builds must feel fast.
- **Memory**: Editor must not consume excessive RAM. Lightweight panels only.
- **Rendering**: GTX 750 Ti is OpenGL 4.5 capable, but only OpenGL 3.3 Core is used for maximum simplicity and portability.
- **Scope**: Feature creep is the primary enemy. Every decision must be weighed against MVP goals.

---

*Keep it small. Keep it working. Ship the thing.*
