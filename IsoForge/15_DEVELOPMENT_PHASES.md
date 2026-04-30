# 15 — Development Phases

Each phase is a small, verifiable milestone. Complete one phase fully before starting the next. No phase should take more than a few days of focused work.

---

## Phase 0: Repository and Documentation Setup

**Goal**: Create the base project structure. No engine code yet.

**Files to create**:
- `CMakeLists.txt` (placeholder, no targets yet)
- `README.md`
- `.gitignore`
- `docs/` (all documentation files)
- `engine/`, `editor/`, `sandbox_project/`, `third_party/`, `tools/` (empty folders with `.gitkeep`)

**Systems touched**: None.

**Acceptance criteria**:
- Repository can be cloned.
- All folders exist.
- `cmake -S . -B build` runs without error (even though it produces nothing yet).
- All documentation files are present in `docs/`.

**Manual test steps**:
1. `git clone` the repository.
2. `ls` to verify folder structure.
3. `cmake -S . -B build` — should succeed with "no targets" warning.

**What not to do**: Do not implement SDL3, OpenGL, or ImGui yet.

---

## Phase 1: SDL3 + OpenGL + ImGui Editor Shell

**Goal**: Open a native window with an OpenGL context and a working Dear ImGui DockSpace layout.

**Files to create / modify**:
- `editor/main.cpp`
- `editor/EditorApp.h`, `editor/EditorApp.cpp`
- `engine/platform/Window.h`, `engine/platform/Window.cpp`
- `engine/platform/OpenGLContext.h`, `engine/platform/OpenGLContext.cpp`
- `engine/core/Log.h`, `engine/core/Log.cpp`
- `editor/CMakeLists.txt`, `engine/CMakeLists.txt`
- Root `CMakeLists.txt` (add targets)
- `third_party/` (SDL3, ImGui, spdlog, glm added)

**Systems touched**: Platform, Logger, EditorApp (main loop).

**Acceptance criteria**:
- Window opens at 1280×720.
- OpenGL 3.3 Core context is active.
- Dear ImGui DockSpace fills the window.
- Basic menu bar visible ("File", "View").
- spdlog logs "Editor started" to stdout.
- Window can be closed cleanly.

**Manual test steps**:
1. `cmake --build build && ./build/IsoForgeEditor`
2. Window opens.
3. ImGui panels can be docked/undocked.
4. Close the window — no crash.

**What not to do**: Do not render any scene content yet. Do not create the framebuffer yet.

---

## Phase 2: Framebuffer Viewport

**Goal**: Create an OpenGL FBO and display it as an ImGui image in the Scene Viewport panel.

**Files to create / modify**:
- `engine/renderer/Framebuffer.h`, `engine/renderer/Framebuffer.cpp`
- `engine/renderer/Renderer.h`, `engine/renderer/Renderer.cpp`
- `editor/panels/SceneViewportPanel.h`, `editor/panels/SceneViewportPanel.cpp`

**Systems touched**: Renderer, Framebuffer, SceneViewportPanel.

**Acceptance criteria**:
- Scene Viewport panel shows a solid-color rectangle (the FBO clear color).
- FBO is correctly sized to match the panel.
- FBO resizes when the panel is resized.
- No rendering artifacts or black screens.

**Manual test steps**:
1. Run editor.
2. Resize the Scene Viewport panel — the color rectangle resizes correctly.
3. No corruption when switching focus between panels.

**What not to do**: Do not draw any scene content into the FBO yet. A solid color is sufficient.

---

## Phase 3: Isometric Grid Rendering

**Goal**: Render a basic isometric grid inside the scene viewport framebuffer.

**Files to create / modify**:
- `engine/iso/IsoMath.h` (header-only coordinate math)
- `engine/iso/IsoGrid.h`, `engine/iso/IsoGrid.cpp`
- `engine/renderer/Renderer.cpp` (add DrawLine method)
- `engine/renderer/Shader.h`, `engine/renderer/Shader.cpp`
- `engine/renderer/shaders/line.vert`, `engine/renderer/shaders/line.frag`

**Systems touched**: Renderer, IsoMath, IsoGrid.

**Acceptance criteria**:
- A 20×20 isometric diamond grid is visible inside the scene viewport.
- Grid lines are rendered correctly with the isometric angle.
- The grid is visible on the FBO color.

**Manual test steps**:
1. Run editor.
2. Scene Viewport shows a grid of isometric diamonds.
3. Grid is centered roughly in the viewport.

**What not to do**: Do not implement camera yet. Do not implement tile painting yet.

---

## Phase 4: Camera Pan and Zoom

**Goal**: Implement the editor camera with pan (middle mouse drag) and zoom (scroll wheel).

**Files to create / modify**:
- `editor/EditorCamera.h`, `editor/EditorCamera.cpp`
- `engine/input/Input.h`, `engine/input/Input.cpp`
- `editor/panels/SceneViewportPanel.cpp` (add input routing)

**Systems touched**: EditorCamera, Input, SceneViewportPanel.

**Acceptance criteria**:
- Middle mouse button drag pans the camera.
- Scroll wheel zooms in and out.
- Input only affects the camera when the mouse is inside the scene viewport.
- Grid rendering updates to reflect camera position and zoom.

**Manual test steps**:
1. Run editor.
2. Middle-click and drag — grid moves.
3. Scroll — grid zooms in/out.
4. Move mouse to ImGui panel — camera input stops.

**What not to do**: Do not implement tile picking yet. Do not implement any keyboard shortcuts yet.

---

## Phase 5: Mouse Hover Tile Detection

**Goal**: Detect which isometric tile the mouse is hovering over and highlight it.

**Files to create / modify**:
- `engine/iso/IsoMath.h` (add ScreenToGrid, GetHoveredTile)
- `editor/EditorSelectionState.h`
- `editor/panels/SceneViewportPanel.cpp` (add tile highlight rendering)
- `engine/iso/IsoGrid.cpp` (add RenderTileHighlight)

**Systems touched**: IsoMath, EditorSelectionState, SceneViewportPanel, IsoGrid.

**Acceptance criteria**:
- The tile under the mouse cursor is highlighted with a semi-transparent color.
- Highlight follows mouse movement correctly.
- Highlight accounts for camera pan and zoom.
- No highlight when mouse is outside the viewport.

**Manual test steps**:
1. Run editor.
2. Move mouse over grid — the diamond under the cursor highlights.
3. Pan and zoom — highlight still follows the correct tile.

**What not to do**: Do not implement tile painting yet.

---

## Phase 6: Console / Log Panel

**Goal**: Add the ConsolePanel with the custom ImGui spdlog sink.

**Files to create / modify**:
- `editor/panels/ConsolePanel.h`, `editor/panels/ConsolePanel.cpp`
- `engine/core/Log.h` (add `AddSink()`)
- `editor/EditorApp.cpp` (register ImGui sink after panel creation)

**Systems touched**: Logger, ConsolePanel.

**Acceptance criteria**:
- Console panel is visible and dockable.
- Engine log messages appear in the console with color coding by level.
- Clear button works.
- Auto-scroll follows new messages.

**Manual test steps**:
1. Run editor.
2. Console panel shows startup log messages.
3. Press Clear — console clears.
4. Perform actions that generate logs — they appear in the console.

**What not to do**: Do not add log filtering by category yet. Do not add file sink yet.

---

## Phase 7: Tilemap Data Structure

**Goal**: Define the tilemap data structure in the engine (no rendering or editing yet).

**Files to create / modify**:
- `engine/scene/components/TilemapComponent.h`
- `engine/iso/Tilemap.h`, `engine/iso/Tilemap.cpp`

**Systems touched**: TilemapComponent, Tilemap data.

**Acceptance criteria**:
- Tilemap can be created with a given width, height, and number of layers.
- Tiles can be set and retrieved by (layer, x, y) coordinates.
- Tile ID 0 means empty.
- Basic unit-testable API (even if no test framework is set up yet).

**Manual test steps**:
1. Create a Tilemap in `main.cpp` or `EditorApp.cpp` initialization.
2. Set a few tiles, read them back, log the values.
3. Values read back match values set.

**What not to do**: Do not implement rendering yet. Do not implement JSON save/load yet.

---

## Phase 8: Tile Palette Panel

**Goal**: Add the TilePalettePanel that displays a tileset and allows selecting a tile.

**Files to create / modify**:
- `editor/panels/TilePalettePanel.h`, `editor/panels/TilePalettePanel.cpp`
- `editor/EditorApp.h` (add TilePaletteState)
- `engine/assets/AssetManager.h`, `engine/assets/AssetManager.cpp`
- `engine/renderer/Texture.h`, `engine/renderer/Texture.cpp`

**Systems touched**: TilePalettePanel, AssetManager, Texture.

**Acceptance criteria**:
- Tile palette panel is visible and shows a placeholder tileset texture.
- Clicking a tile highlights it as selected.
- Selected tile ID is stored in TilePaletteState.

**Manual test steps**:
1. Place a test tileset PNG in `sandbox_project/assets/textures/`.
2. Run editor.
3. Tile palette shows the tileset grid.
4. Click a tile — it highlights.

**What not to do**: Do not implement tile painting yet.

---

## Phase 9: Tile Painting and Erasing

**Goal**: Paint tiles onto the tilemap by clicking in the scene viewport, and erase by right-clicking.

**Files to create / modify**:
- `editor/panels/SceneViewportPanel.cpp` (add paint/erase input handling)
- `engine/iso/IsoGrid.cpp` (add tilemap render pass)

**Systems touched**: SceneViewportPanel, TilemapComponent, IsoGrid renderer.

**Acceptance criteria**:
- Left-click on a tile paints the selected tile from the palette.
- Right-click erases (sets tile ID to 0).
- Painted tiles are visible in the viewport.
- Paint and erase respect the camera position and zoom.

**Manual test steps**:
1. Run editor.
2. Select a tile in the palette.
3. Left-click on the grid — tile appears.
4. Right-click — tile disappears.

**What not to do**: Do not implement save/load yet. Do not implement undo yet.

---

## Phase 10: Tilemap JSON Save/Load

**Goal**: Save the tilemap to a JSON file and reload it.

**Files to create / modify**:
- `engine/serialization/TilemapSerializer.h`, `engine/serialization/TilemapSerializer.cpp`
- `sandbox_project/tilemaps/test_tilemap.json` (generated on save)
- `editor/EditorApp.cpp` (add Save/Load menu actions)

**Systems touched**: Serialization, TilemapSerializer, MainMenuBar.

**Acceptance criteria**:
- File → Save saves the tilemap to `sandbox_project/tilemaps/test_tilemap.json`.
- Reopening the editor and loading the file restores the tilemap.
- The JSON file matches the format in `09_SERIALIZATION.md`.

**Manual test steps**:
1. Run editor, paint some tiles.
2. File → Save.
3. Close editor, reopen.
4. File → Load — tiles are restored.

**What not to do**: Do not implement scene save/load yet. Do not implement undo yet.

---

## Phase 11: EnTT Scene Foundation

**Goal**: Integrate EnTT, create the Scene class and Entity wrapper, and create a basic entity at startup.

**Files to create / modify**:
- `engine/scene/Scene.h`, `engine/scene/Scene.cpp`
- `engine/scene/Entity.h`
- `engine/scene/components/NameComponent.h`
- `engine/scene/components/TransformComponent.h`
- `editor/EditorApp.cpp` (create Scene, add test entity)

**Systems touched**: Scene, Entity, ECS.

**Acceptance criteria**:
- Scene can be created.
- `CreateEntity("Test")` returns a valid entity with NameComponent and TransformComponent.
- `DestroyEntity()` removes the entity.
- Scene can be iterated via `View<NameComponent>()`.

**Manual test steps**:
1. Run editor.
2. Log all entity names at startup — "Test" should appear.
3. Destroy entity — name no longer appears in log.

**What not to do**: Do not add hierarchy panel yet. Do not add inspector yet.

---

## Phase 12: Hierarchy Panel

**Goal**: Display all scene entities in the HierarchyPanel. Allow selecting an entity by clicking.

**Files to create / modify**:
- `editor/panels/HierarchyPanel.h`, `editor/panels/HierarchyPanel.cpp`
- `editor/EditorSelectionState.h` (add `selectedEntity`)

**Systems touched**: HierarchyPanel, EditorSelectionState, Scene.

**Acceptance criteria**:
- All entities appear in the hierarchy by name.
- Clicking an entity selects it (stored in EditorSelectionState).
- Selected entity is highlighted in the list.
- "Create Entity" button adds a new entity with a default name.
- Right-click → Delete removes the entity from the scene.

**Manual test steps**:
1. Run editor with 3 test entities.
2. Hierarchy shows all 3 names.
3. Click one — it highlights.
4. Create a new entity via button — appears in list.
5. Delete one — it disappears.

---

## Phase 13: Inspector Panel

**Goal**: Display and edit the components of the selected entity in the InspectorPanel.

**Files to create / modify**:
- `editor/panels/InspectorPanel.h`, `editor/panels/InspectorPanel.cpp`
- `engine/scene/components/SpriteRendererComponent.h`

**Systems touched**: InspectorPanel, EditorSelectionState, Scene.

**Acceptance criteria**:
- Inspector shows "No entity selected" when nothing is selected.
- Inspector shows NameComponent (editable text).
- Inspector shows TransformComponent (editable position and scale).
- Changes in the inspector are reflected immediately in the scene.

**Manual test steps**:
1. Select an entity in the hierarchy.
2. Inspector shows its name and transform.
3. Edit position values — entity position updates.
4. Edit name — hierarchy name updates.

---

## Phase 14: Sprite Rendering

**Goal**: Render entities with SpriteRendererComponent as textured quads in the scene viewport.

**Files to create / modify**:
- `engine/renderer/Renderer.cpp` (add DrawSprite / DrawTexturedQuad)
- `engine/renderer/VertexArray.h`, `engine/renderer/VertexArray.cpp`
- `engine/renderer/shaders/quad.vert`, `engine/renderer/shaders/quad.frag`
- `editor/panels/InspectorPanel.cpp` (add SpriteRenderer section)

**Systems touched**: Renderer, SpriteRendererComponent, Scene render pass.

**Acceptance criteria**:
- Entities with SpriteRendererComponent display their texture in the viewport.
- Missing texture shows the magenta/black checkerboard.
- Color tint is applied correctly.
- Sprites are positioned at their TransformComponent.position.

**Manual test steps**:
1. Create an entity with a SpriteRenderer, assign a texture.
2. Entity appears in viewport as a textured quad.
3. Move the transform in the inspector — sprite moves.
4. Assign invalid texture path — checkerboard fallback appears.

---

## Phase 15: Scene Save/Load

**Goal**: Save and load the full scene (entities, components) as a JSON file.

**Files to create / modify**:
- `engine/serialization/SceneSerializer.h`, `engine/serialization/SceneSerializer.cpp`
- `sandbox_project/scenes/main_scene.json` (generated on save)
- `editor/EditorApp.cpp` (add scene save/load menu actions)

**Systems touched**: Serialization, SceneSerializer, Scene, MainMenuBar.

**Acceptance criteria**:
- File → Save Scene writes `main_scene.json`.
- File → Load Scene reads `main_scene.json` and recreates all entities with their components.
- Entity names, transforms, and sprite renderer data survive save/load.
- Missing entities in JSON are handled gracefully.

**Manual test steps**:
1. Create entities, set positions and sprites.
2. File → Save Scene.
3. Close editor, reopen, File → Load Scene.
4. All entities appear with their saved data.
