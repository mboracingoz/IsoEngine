# 18 — Workflow & Working Diagrams

This file documents the **practical working flow** of the isometric tile-map editor and its runtime engine.  
All diagrams are written in Mermaid and are intended to be implementation-oriented — every node maps to a real module, class, or decision point that Codex must respect when writing code.

---

## Cross-Reference

| Related Document | Topic |
|---|---|
| [03_ARCHITECTURE.md](03_ARCHITECTURE.md) | Module layout and layer boundaries |
| [05_RUNTIME_EDITOR_SEPARATION.md](05_RUNTIME_EDITOR_SEPARATION.md) | Editor / runtime boundary rules |
| [06_RENDERING_AND_VIEWPORT.md](06_RENDERING_AND_VIEWPORT.md) | Framebuffer, Renderer2D, OpenGL context |
| [07_ISOMETRIC_SYSTEM.md](07_ISOMETRIC_SYSTEM.md) | Isometric coordinate math |
| [08_SCENE_AND_ECS.md](08_SCENE_AND_ECS.md) | EnTT scene, entity lifecycle |
| [09_SERIALIZATION.md](09_SERIALIZATION.md) | Scene / tilemap JSON format |
| [11_EDITOR_PANELS.md](11_EDITOR_PANELS.md) | Panel responsibilities and APIs |
| [15_DEVELOPMENT_PHASES.md](15_DEVELOPMENT_PHASES.md) | MVP roadmap phases |
| [16_CODEX_TASK_RULES.md](16_CODEX_TASK_RULES.md) | Rules for Codex task execution |

---

## 1 — Overall Editor Working Flow

Shows the complete lifecycle from process start to clean shutdown.

```mermaid
flowchart TD
    A([Start EditorApp]) --> B[Parse CLI args / config]
    B --> C[Initialize SDL3 window]
    C --> D[Initialize OpenGL context]
    D --> E[Initialize ImGui\nwith docking enabled]
    E --> F[Load editor settings\nfrom editor_settings.json]
    F --> G[Create EditorContext\nSelectionContext · ProjectContext]
    G --> H[Create editor panels\nHierarchy · Inspector · TilePalette\nAssetBrowser · Console · SceneViewport]
    H --> I[Create SceneFramebuffer\ncolor + depth attachments]
    I --> J[Load or create default Scene]
    J --> K{Enter main loop}

    K --> L[Poll SDL3 events]
    L --> M{SDL_QUIT\nreceived?}
    M -- Yes --> Z
    M -- No --> N[Feed events to ImGui]
    N --> O{Is mouse over\nImGui panel?}
    O -- Yes --> P[ImGui consumes input]
    O -- No --> Q[Route input to\nSceneViewportPanel]
    P --> R[Update editor state\ne.g. selection · panel flags]
    Q --> R
    R --> S{Scene dirty\nor playing?}
    S -- Yes --> T[Update Scene / ECS]
    S -- No --> U
    T --> U[Bind Framebuffer]
    U --> V[Renderer2D clears + renders Scene]
    V --> W[Unbind Framebuffer]
    W --> X[Render ImGui editor UI\nall panels · docking]
    X --> Y[SceneViewportPanel submits\nframebuffer texture via ImGui::Image]
    Y --> AA[SDL_GL_SwapWindow]
    AA --> K

    Z([Shutdown editor\nImGui · SDL3 · OpenGL])
```

---

## 2 — Frame Loop Working Diagram

Shows every step that executes once per rendered frame.

```mermaid
flowchart TD
    A([Begin frame]) --> B[SDL_PollEvent loop]
    B --> C[ImGui_ImplSDL3_ProcessEvent]
    C --> D[ImGui::NewFrame]
    D --> E[Calculate delta time\nusing SDL_GetTicks or std::chrono]
    E --> F{Viewport focused\nor hovered?}
    F -- Yes --> G[CameraController::Update\npan · zoom · bounds clamp]
    F -- No --> H
    G --> H[Update editor tools\ne.g. TilePainter · SelectionTool]
    H --> I[Bind SceneFramebuffer]
    I --> J[Renderer2D::Clear\nbackground color]
    J --> K[Scene::Render\nsprites · tilemap · grid overlay]
    K --> L[Render viewport overlay\nhover highlight · selection gizmo]
    L --> M[Unbind SceneFramebuffer]
    M --> N[ImGui docking layout\nBeginDockspace]
    N --> O[Render all editor panels\nHierarchy · Inspector · TilePalette\nAssetBrowser · Console]
    O --> P[SceneViewportPanel::OnImGuiRender\nImGui::Image with framebuffer texture ID]
    P --> Q[ImGui::Render\nImGui_ImplOpenGL3_RenderDrawData]
    Q --> R[SDL_GL_SwapWindow]
    R --> S([End frame])
```

---

## 3 — Scene Viewport Input Routing Diagram

Shows how every mouse/keyboard event is dispatched to the correct consumer.

```mermaid
flowchart TD
    A([Mouse / keyboard event\nfrom SDL3]) --> B[ImGui_ImplSDL3_ProcessEvent]
    B --> C{ImGui::GetIO\n.WantCaptureMouse?}
    C -- Yes --> D[Editor UI consumes input\ne.g. panel drag · menu click]
    D --> Z([Event consumed])
    C -- No --> E{Mouse position\nover SceneViewportPanel\nbounds?}
    E -- No --> F[Event ignored\nfor scene purposes]
    F --> Z
    E -- Yes --> G{Is SceneViewportPanel\nfocused / hovered?}
    G -- No --> H[ImGui::SetNextWindowFocus\nwait for click to focus]
    H --> Z
    G -- Yes --> I{Which input type?}
    I -- Middle mouse drag\nor Alt+LMB --> J[CameraController: Pan]
    I -- Scroll wheel --> K[CameraController: Zoom]
    I -- R key\nand not typing --> S[CameraController: Reset]
    I -- Mouse move --> L[IsoPicker: ScreenToGrid\ncompute hovered tile]
    L --> M[TilemapData: show\nhover highlight]
    I -- Left click --> N[TilePainter: paint\nselected tile at grid coord]
    N --> O[Mark TilemapData dirty]
    I -- Right click --> P[TilePainter: erase\ntile at grid coord]
    P --> O
    I -- Left click\non entity --> Q[SelectionContext:\nset selected entity]
    Q --> R[InspectorPanel refreshes]
    J & K & S & M & O & R --> Z
```

Phase 2C note:
- `R` resets the Scene Viewport camera only while the viewport is hovered or focused, and it is ignored while typing into an ImGui text field.

Phase 2D note:
- Hovered tile detection converts the mouse position from framebuffer-image space into viewport-local coordinates, then into engine-side isometric grid coordinates.
- The hovered tile highlight may be rendered from the previous frame's stored hover result so the framebuffer pass can still run before `ImGui::Image()`.

Phase 3D note:
- `File > Save Tilemap` and `File > Load Tilemap` currently operate only on the active debug `TilemapData` using the fixed MVP path `sandbox_project/tilemaps/debug_tilemap.json`.
- This workflow is intentionally tilemap-only and is not the final scene serialization pipeline.

Phase 3E note:
- `File > Save Tilemap` now reuses the current tilemap path when available.
- `File > Save Tilemap As...` and `File > Load Tilemap` use small ImGui modals scoped to `sandbox_project/tilemaps/`.
- The active tilemap path is editor-visible, but this remains project-local tilemap save/load rather than full scene persistence.

---

## 4 — Isometric Tile Painting Workflow

Shows the full path from user gesture to tilemap mutation.

```mermaid
flowchart TD
    A([User selects tile\nin TilePalettePanel]) --> B[SelectionContext stores\nselected tile ID + texture region]
    B --> C[User moves mouse over\nSceneViewportPanel]
    C --> D[Read raw SDL mouse position\nabs screen coords]
    D --> E[Subtract SceneViewportPanel\nwindow position → viewport local pos]
    E --> F[Apply inverse camera transform\noffset by CameraController position\ndivide by zoom]
    F --> G[IsoPicker::ScreenToGrid\ngridX = screenY/halfH + screenX/halfW / 2\ngridY = screenY/halfH - screenX/halfW / 2]
    G --> H{Is grid coord\nwithin TilemapData\nbounds?}
    H -- No --> I[Clamp or discard\nno highlight]
    H -- Yes --> J[TilemapData::SetHoverTile\nrender highlight quad]
    J --> K{Left mouse\nbutton down?}
    K -- Yes --> L[TilemapData::SetTile\ngridX·gridY · tileID]
    K -- No --> M{Right mouse\nbutton down?}
    M -- Yes --> N[TilemapData::EraseTile\ngridX·gridY]
    M -- No --> O[No mutation\nthis frame]
    L --> P[Mark TilemapData dirty\nisDirty = true]
    N --> P
    P --> Q[Renderer2D re-renders\ntilemap next frame]
    Q --> R{Auto-save\nenabled?}
    R -- Yes --> S[Schedule async\nJSON write]
    R -- No --> T([Wait for\nexplicit Save])
    S --> T
    O --> U([End — hover only])
```

---

## 5 — Scene Save / Load Workflow

### 5a — Save Flow

```mermaid
flowchart TD
    A([User clicks\nSave Scene]) --> B[SceneSerializer::Serialize\nreceives Scene reference]
    B --> C[Iterate all EnTT entities\nvia registry.view]
    C --> D[Serialize components\nTransform · SpriteRenderer\nTagComponent · etc.]
    D --> E[SceneSerializer collects\nTilemapData layer array\ntile IDs · dimensions · tilesetPath]
    E --> F[Serialize scene metadata\nname · version · editorCameraState]
    F --> G[nlohmann::json assembles\nfull JSON object]
    G --> H[Write to scene.json\nvia std::ofstream]
    H --> I{Write\nsucceeded?}
    I -- Yes --> J[ConsoleLogSink: LOG_INFO\n'Scene saved: path']
    I -- No --> K[ConsoleLogSink: LOG_ERROR\n'Failed to write: errno']
    J & K --> L([End save])
```

### 5b — Load Flow

```mermaid
flowchart TD
    A([User opens scene\nor File › Open]) --> B[Read scene.json\nstd::ifstream → nlohmann::json]
    B --> C{JSON parse\nsucceeded?}
    C -- No --> D[LOG_ERROR: invalid JSON\nAbort load]
    D --> Z([End — failed])
    C -- Yes --> E{Version field\ncompatible?}
    E -- No --> F[LOG_WARN: version mismatch\nAttempt migration or abort]
    F --> Z
    E -- Yes --> G[Scene::Clear\ndestroy existing entities]
    G --> H[SceneSerializer::Deserialize\niterates entities array]
    H --> I[registry.create\nfor each entity]
    I --> J[Restore components\nTransform · SpriteRenderer · Tag]
    J --> K[SceneSerializer restores\nTilemapData\ntile IDs · dimensions]
    K --> L[AssetManager::Resolve\ntileset and sprite paths]
    L --> M{Any asset\nnot found?}
    M -- Yes --> N[LOG_WARN per missing asset\ninsert placeholder texture]
    M -- No --> O
    N --> O[Camera restored\nfrom editorCameraState]
    O --> P[Scene displayed\nin SceneViewportPanel]
    P --> Q([End — load complete])
```

---

## 6 — Runtime / Editor Separation Workflow

Shows the strict boundary between editor code and runtime/engine code.

```mermaid
graph TD
    subgraph Editor ["editor/ (Editor Layer)"]
        EA[EditorApp]
        EP[Editor Panels\nHierarchy · Inspector\nTilePalette · Console]
        EC[EditorContext\nSelectionContext\nProjectContext]
    end

    subgraph Engine ["engine/ (Runtime Layer)"]
        SC[Scene]
        REG[EnTT Registry]
        TD[TilemapData]
        R2D[Renderer2D]
        FB[Framebuffer]
        SER[SceneSerializer]
        AM[AssetManager]
        CAM[CameraController]
    end

    subgraph ThirdParty ["third_party/"]
        SDL3
        OpenGL
        ImGui
        EnTT
        nlohmann
    end

    EA --> EC
    EA --> EP
    EA --> SC
    EP --> EC
    EC --> SC
    EC --> AM

    SC --> REG
    SC --> TD
    SC --> R2D
    SER --> SC
    R2D --> FB
    R2D --> OpenGL
    FB --> OpenGL
    CAM --> SC

    EA --> SDL3
    EA --> ImGui
    Engine --> EnTT
    Engine --> nlohmann
    Engine --> SDL3

    SC -.->|must NOT reference| EP
    SC -.->|must NOT reference| EA
    Engine -.->|must NOT include\neditor headers| Editor

    style Editor fill:#1a3a5c,color:#fff
    style Engine fill:#1a4a2a,color:#fff
    style ThirdParty fill:#3a2a1a,color:#fff
```

---

## 7 — Dependency Direction Diagram

Enforces the one-way dependency rule across all project layers.

```mermaid
graph TD
    SBX[sandbox_project\n/ game]
    ED[editor]
    ENG[engine]
    TP[third_party\nSDL3 · OpenGL · ImGui\nEnTT · nlohmann · stb]

    SBX -->|depends on| ENG
    ED -->|depends on| ENG
    ENG -->|depends on| TP
    TP -->|depends on nothing\ninside project| NONE[ ]

    ENG -.->|MUST NOT depend on| ED
    ENG -.->|MUST NOT depend on| SBX
    ED -.->|MUST NOT depend on\nSBX internals directly| SBX
    ED -->|accesses SBX only\nthrough Project API| ENG

    style NONE fill:none,stroke:none
    style ENG fill:#1a4a2a,color:#fff
    style ED fill:#1a3a5c,color:#fff
    style SBX fill:#4a1a1a,color:#fff
    style TP fill:#3a2a1a,color:#fff
```

---

## 8 — Codex Task Workflow Diagram

The process Codex must follow for every assigned implementation task.

```mermaid
flowchart TD
    A([Receive task]) --> B[Read relevant\ndocumentation files]
    B --> C[Read current\nproject source files\nin affected modules]
    C --> D[Identify current\nroadmap phase\nfrom 15_DEVELOPMENT_PHASES.md]
    D --> E{Is task within\ncurrent or already\ncompleted phase?}
    E -- No --> F[STOP\nReport: phase not reached\nDo not implement]
    E -- Yes --> G[Modify only\nallowed files\nper 16_CODEX_TASK_RULES.md]
    G --> H[Run CMake build\ncmake --build build]
    H --> I{Build\nsucceeded?}
    I -- No --> J[Fix compile errors\nDo not move on]
    J --> H
    I -- Yes --> K[Run manual or\nautomated test]
    K --> L{Test\npassed?}
    L -- No --> M[Diagnose failure\napply fix]
    M --> H
    L -- Yes --> N[Report list of\nchanged files]
    N --> O[Report test result\nand observations]
    O --> P([STOP\nWait for next task])
    F --> P
```

---

## 9 — MVP Development Roadmap Diagram

Visual representation of the phased build plan.

```mermaid
flowchart LR
    P0([Phase 0\nRepo & Docs\nSetup]) --> P1
    P1([Phase 1\nSDL3 + OpenGL\n+ ImGui Shell]) --> P2
    P2([Phase 2\nFramebuffer\nViewport]) --> P3
    P3([Phase 3\nIsometric Grid\nRendering]) --> P4
    P4([Phase 4\nCamera Pan\n& Zoom]) --> P5
    P5([Phase 5\nMouse Hover\nTile Detection]) --> P6
    P6([Phase 6\nConsole /\nLog Panel]) --> P7
    P7([Phase 7\nTilemap Data\nStructure]) --> P8
    P8([Phase 8\nTile Palette\nPanel]) --> P9
    P9([Phase 9\nTile Painting\n& Erasing]) --> P10
    P10([Phase 10\nTilemap JSON\nSave / Load]) --> P11
    P11([Phase 11\nEnTT Scene\nFoundation]) --> P12
    P12([Phase 12\nHierarchy\nPanel]) --> P13
    P13([Phase 13\nInspector\nPanel]) --> P14
    P14([Phase 14\nSprite\nRendering]) --> P15
    P15([Phase 15\nScene\nSave / Load])

    style P0 fill:#2a2a4a,color:#fff
    style P1 fill:#1a3a5c,color:#fff
    style P2 fill:#1a3a5c,color:#fff
    style P3 fill:#1a4a3a,color:#fff
    style P4 fill:#1a4a3a,color:#fff
    style P5 fill:#1a4a3a,color:#fff
    style P6 fill:#3a3a1a,color:#fff
    style P7 fill:#3a2a1a,color:#fff
    style P8 fill:#3a2a1a,color:#fff
    style P9 fill:#3a2a1a,color:#fff
    style P10 fill:#3a2a1a,color:#fff
    style P11 fill:#4a1a2a,color:#fff
    style P12 fill:#4a1a2a,color:#fff
    style P13 fill:#4a1a2a,color:#fff
    style P14 fill:#4a1a2a,color:#fff
    style P15 fill:#4a1a2a,color:#fff
```

---

## 10 — Editor Panel Communication Diagram

Shows how panels share state through context objects without directly coupling to each other.

```mermaid
graph TD
    subgraph Contexts ["Shared Context Objects (owned by EditorApp)"]
        ECTX[EditorContext]
        SCTX[SelectionContext]
        PCTX[ProjectContext]
        AM[AssetManager]
        CLS[ConsoleLogSink]
    end

    subgraph Scene ["Runtime Scene (owned by EditorApp)"]
        SC[Scene]
        REG[EnTT Registry]
        TD[TilemapData]
        SC --> REG
        SC --> TD
    end

    subgraph Panels ["Editor Panels"]
        SVP[SceneViewportPanel]
        HP[HierarchyPanel]
        IP[InspectorPanel]
        ABP[AssetBrowserPanel]
        TPP[TilePalettePanel]
        CP[ConsolePanel]
    end

    ECTX --> SC
    ECTX --> SCTX
    ECTX --> PCTX
    ECTX --> AM

    SVP --> ECTX
    SVP --> SCTX
    SVP --> SC

    HP --> ECTX
    HP --> SCTX
    HP --> SC

    IP --> SCTX
    IP --> SC

    ABP --> AM
    ABP --> PCTX

    TPP --> SCTX

    CP --> CLS
    CLS -.->|receives log events| CP

    IP -.->|edits selected entity\nthrough EnTT registry API| REG
    SVP -.->|paints tiles through\nTilemapData API| TD
    TPP -.->|writes selected tile ID\nto SelectionContext| SCTX

    HP -.->|Panels must NOT\ndirectly own each other| IP
```

---

## 11 — OpenGL Framebuffer Viewport Sequence Diagram

Shows the frame-level sequence from render start to visible pixel on screen.

```mermaid
sequenceDiagram
    participant EA  as EditorApp
    participant SVP as SceneViewportPanel
    participant FB  as Framebuffer
    participant R2D as Renderer2D
    participant SC  as Scene
    participant IG  as ImGui

    EA->>SVP: OnUpdate(deltaTime)
    SVP->>SVP: Check ImGui panel size
    SVP->>FB: ResizeIfNeeded(width, height)
    FB-->>SVP: Framebuffer resized (or no-op)

    EA->>FB: Bind()
    FB->>FB: glBindFramebuffer(GL_FRAMEBUFFER, fbo)

    EA->>R2D: Clear(backgroundColor)
    R2D->>R2D: glClear(COLOR | DEPTH)

    EA->>SC: Render(cameraTransform)
    SC->>R2D: Submit tilemap quads
    SC->>R2D: Submit sprite quads
    R2D->>R2D: Flush batched draw calls → OpenGL

    EA->>FB: Unbind()
    FB->>FB: glBindFramebuffer(GL_FRAMEBUFFER, 0)

    EA->>IG: NewFrame()
    IG->>IG: Build docking layout

    EA->>SVP: OnImGuiRender()
    SVP->>IG: ImGui::Image(framebuffer.GetColorTextureID(), size)
    IG->>IG: Enqueue image draw call

    EA->>IG: Render()
    IG->>IG: ImGui_ImplOpenGL3_RenderDrawData

    EA->>EA: SDL_GL_SwapWindow
```

---

## 12 — Isometric Coordinate Conversion Diagram

Shows each transformation stage from raw mouse position to final tile index.

```mermaid
flowchart TD
    A([Mouse screen position\nSDL_GetMouseState\nabsX · absY]) --> B[Subtract SceneViewportPanel\nwindow position\n→ viewport local pos\nvpX = absX - panelX\nvpY = absY - panelY]
    B --> C[Apply inverse camera transform\nworldX = vpX / zoom - camOffsetX\nworldY = vpY / zoom - camOffsetY]
    C --> D["IsoPicker::ScreenToGrid\ngridX = (worldY / halfTileH + worldX / halfTileW) / 2\ngridY = (worldY / halfTileH - worldX / halfTileW) / 2\nwhere halfTileW = tileWidth / 2\n      halfTileH = tileHeight / 2"]
    D --> E[floor to integer\ntileCol = floor(gridX)\ntileRow = floor(gridY)]
    E --> F{Col and Row\nwithin TilemapData\nbounds?}
    F -- No --> G([Discard — out of bounds])
    F -- Yes --> H[TileIndex = tileRow * mapWidth + tileCol]
    H --> I[TilemapData::GetTile / SetTile\nusing TileIndex]
    I --> J[Renderer2D draws\nhover highlight quad at\nscreenX = tileCol − tileRow × halfTileW\nscreenY = tileCol + tileRow × halfTileH]
    J --> K([Tile highlighted / painted])
```

### Coordinate Formulas

**Grid → Screen** (used by Renderer2D to position quads):

```
screenX = (gridX - gridY) * tileWidth  / 2
screenY = (gridX + gridY) * tileHeight / 2
```

**Screen → Grid** (used by IsoPicker to pick tiles):

```
gridX = (screenY / (tileHeight / 2) + screenX / (tileWidth / 2)) / 2
gridY = (screenY / (tileHeight / 2) - screenX / (tileWidth / 2)) / 2
```

> **Important:** `screenX` and `screenY` in the Screen → Grid formula are **world-space** coordinates.  
> Before calling `ScreenToGrid`, the raw viewport-local mouse position must be adjusted by:
> 1. Subtracting the `SceneViewportPanel` offset to get viewport-local coordinates.
> 2. Dividing by `zoom` and adding the `CameraController` world offset to get world-space coordinates.  
> Skipping either step produces incorrect tile picks whenever the camera is panned or zoomed.

---

## How Codex Should Use These Diagrams

### 1 — Follow workflow order

Every diagram encodes a strict execution sequence. Codex must implement functions and calls in the order shown. For example, `Framebuffer::Bind` must happen **before** any `Renderer2D` draw calls, and `Framebuffer::Unbind` must happen **before** `ImGui::Image` is called. Reversing or skipping steps will produce black viewports or incorrect renders.

### 2 — Do not implement systems before their roadmap phase

Diagram 9 (MVP Roadmap) is the gating document. Codex must not write `TilemapData`, `TilePainter`, or `IsoPicker` code during Phase 1 (SDL3 shell), even if those classes are referenced in other diagrams. Each phase must compile and pass its own tests before the next phase begins.

### 3 — Use diagrams to understand allowed dependencies

Diagram 7 (Dependency Direction) is the law. If Codex finds itself `#include`-ing an editor header inside `engine/`, that is a dependency inversion and must be refactored. Diagram 6 (Runtime/Editor Separation) further clarifies which objects may hold references to which. When in doubt, check those two diagrams before adding any `#include`.

### 4 — Update diagrams when architecture changes

If a refactor changes the execution order, adds a new context object, or moves a module between layers, the corresponding diagram(s) in this file must be updated in the same commit. Stale diagrams will cause Codex (and human developers) to make incorrect assumptions. Treat diagrams as **living documentation**, not optional commentary.
