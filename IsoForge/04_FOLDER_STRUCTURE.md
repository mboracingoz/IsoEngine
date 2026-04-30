# 04 — Folder Structure

---

## Top-Level Layout

```
IsoForgeEditor/
├── CMakeLists.txt          ← Root build file
├── README.md
├── .gitignore
├── docs/                   ← All documentation (this folder)
├── editor/                 ← Editor-only source code
├── engine/                 ← Runtime engine source code
├── sandbox_project/        ← Sample project data (not compiled)
├── third_party/            ← External library source or integration notes
└── tools/                  ← Optional helper scripts
```

---

## Full Expanded Structure

```
IsoForgeEditor/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── docs/
│   ├── 00_PROJECT_OVERVIEW.md
│   ├── 01_REQUIREMENTS.md
│   ├── 02_TECH_STACK.md
│   ├── 03_ARCHITECTURE.md
│   ├── 04_FOLDER_STRUCTURE.md
│   ├── 05_RUNTIME_EDITOR_SEPARATION.md
│   ├── 06_RENDERING_AND_VIEWPORT.md
│   ├── 07_ISOMETRIC_SYSTEM.md
│   ├── 08_SCENE_AND_ECS.md
│   ├── 09_SERIALIZATION.md
│   ├── 10_ASSET_SYSTEM.md
│   ├── 11_EDITOR_PANELS.md
│   ├── 12_LOGGING_AND_CONSOLE.md
│   ├── 13_BUILD_SYSTEM.md
│   ├── 14_PERFORMANCE_GUIDELINES.md
│   ├── 15_DEVELOPMENT_PHASES.md
│   ├── 16_CODEX_TASK_RULES.md
│   └── 17_FIRST_IMPLEMENTATION_TASK.md
│
├── engine/
│   ├── CMakeLists.txt
│   ├── core/
│   │   ├── Log.h               ← Logger init and macros
│   │   ├── Log.cpp
│   │   ├── Assert.h            ← ISOFORGE_ASSERT macros
│   │   └── Types.h             ← Common typedefs (u32, f32, etc.)
│   ├── platform/
│   │   ├── Window.h            ← SDL3 window wrapper
│   │   ├── Window.cpp
│   │   ├── OpenGLContext.h     ← GL context setup
│   │   └── OpenGLContext.cpp
│   ├── input/
│   │   ├── Input.h             ← Input state queries
│   │   └── Input.cpp
│   ├── renderer/
│   │   ├── Renderer.h          ← Main renderer class
│   │   ├── Renderer.cpp
│   │   ├── Framebuffer.h       ← FBO wrapper
│   │   ├── Framebuffer.cpp
│   │   ├── Shader.h            ← Shader compilation/linking
│   │   ├── Shader.cpp
│   │   ├── Texture.h           ← GPU texture object
│   │   ├── Texture.cpp
│   │   ├── VertexArray.h       ← VAO wrapper
│   │   ├── VertexArray.cpp
│   │   └── shaders/
│   │       ├── quad.vert
│   │       └── quad.frag
│   ├── iso/
│   │   ├── IsoGrid.h           ← Grid rendering
│   │   ├── IsoGrid.cpp
│   │   ├── IsoMath.h           ← Coordinate conversion (header-only math)
│   │   └── Tilemap.h / .cpp    ← Tilemap data structure
│   ├── scene/
│   │   ├── Scene.h             ← Scene class (owns entt::registry)
│   │   ├── Scene.cpp
│   │   ├── Entity.h            ← Entity wrapper around entt::entity
│   │   └── components/
│   │       ├── NameComponent.h
│   │       ├── TransformComponent.h
│   │       ├── SpriteRendererComponent.h
│   │       ├── CameraComponent.h
│   │       └── TilemapComponent.h
│   ├── assets/
│   │   ├── AssetManager.h
│   │   └── AssetManager.cpp
│   └── serialization/
│       ├── SceneSerializer.h
│       ├── SceneSerializer.cpp
│       ├── TilemapSerializer.h
│       └── TilemapSerializer.cpp
│
├── editor/
│   ├── CMakeLists.txt
│   ├── main.cpp                ← Entry point
│   ├── EditorApp.h             ← Top-level editor application
│   ├── EditorApp.cpp
│   ├── EditorCamera.h          ← Editor-only viewport camera
│   ├── EditorCamera.cpp
│   ├── EditorSelectionState.h  ← Currently selected entity, tile, etc.
│   └── panels/
│       ├── SceneViewportPanel.h / .cpp
│       ├── HierarchyPanel.h / .cpp
│       ├── InspectorPanel.h / .cpp
│       ├── AssetBrowserPanel.h / .cpp
│       ├── TilePalettePanel.h / .cpp
│       └── ConsolePanel.h / .cpp
│
├── sandbox_project/
│   ├── project.json
│   ├── scenes/
│   │   └── main_scene.json
│   ├── tilemaps/
│   │   └── test_tilemap.json
│   └── assets/
│       └── textures/
│           └── tileset_placeholder.png
│
├── third_party/
│   ├── README.md               ← Notes on how each library is integrated
│   ├── SDL3/                   ← Either vendored or FetchContent-managed
│   ├── imgui/                  ← Dear ImGui (docking branch)
│   ├── imguizmo/               ← ImGuizmo
│   ├── entt/                   ← EnTT (header-only)
│   ├── nlohmann/               ← nlohmann/json (header-only)
│   ├── spdlog/                 ← spdlog
│   ├── stb/                    ← stb_image.h
│   └── glm/                    ← GLM (header-only)
│
└── tools/
    ├── build.sh                ← Simple build helper script (bash)
    └── clean.sh                ← Clean build directory
```

---

## Folder Explanations

### `engine/`

Contains all reusable runtime engine code. This code should eventually be usable without the editor.

**Belongs here**:
- Platform abstraction (window, OpenGL context, input)
- Renderer (OpenGL draw calls, shaders, framebuffer, textures)
- Isometric math and grid system
- Scene and ECS (EnTT registry, entity wrapper, components)
- Asset manager (texture loading and caching)
- Serialization (scene and tilemap JSON read/write)
- Core utilities (logger initialization, type aliases)

**Does not belong here**:
- ImGui calls
- Editor panel logic
- Editor camera
- Editor selection state
- main.cpp

---

### `editor/`

Contains all editor-only code. Nothing in this folder should be needed to ship a game.

**Belongs here**:
- `main.cpp` — the editor entry point
- `EditorApp` — the top-level application class that owns the main loop
- All ImGui panel classes
- Editor camera (different from any in-game camera)
- Editor selection state (which entity is selected, which tile is selected)
- Editor commands (undo/redo — later)
- Editor settings persistence (later)

**Does not belong here**:
- Engine systems (renderer, scene, ECS, assets)
- Game logic
- Isometric math (pure math belongs in `engine/iso/IsoMath.h`)

---

### `sandbox_project/`

Contains a sample project used for development and testing. This is runtime data, not compiled code.

**Belongs here**:
- `project.json` — project metadata and settings
- `scenes/*.json` — serialized scene files
- `tilemaps/*.json` — serialized tilemap files
- `assets/textures/` — texture files used by the sample project

**Does not belong here**:
- C++ source code
- CMakeLists.txt
- Build artifacts

---

### `third_party/`

Contains external library source code or FetchContent integration stubs. See `13_BUILD_SYSTEM.md` for details on the integration strategy.

**Belongs here**:
- Vendored library source (preferred for stability)
- A `README.md` explaining each library's version and source

**Does not belong here**:
- Modified versions of third-party code without clear documentation of the change
- Libraries that are not actually used

---

### `docs/`

All project documentation. This folder is the source of truth for architecture decisions.

**Belongs here**:
- All Markdown documentation files
- Architecture diagrams (as Mermaid in Markdown)

**Does not belong here**:
- Source code
- Build artifacts

---

### `tools/`

Optional helper scripts. These are conveniences for the developer, not part of the build system.

**Examples**:
- `build.sh` — wraps the CMake build commands
- `clean.sh` — removes the build directory

These scripts are not required for the project to build. They are personal workflow helpers.
