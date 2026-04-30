# 11 — Editor Panels

---

## Overview

The editor is built from a set of ImGui panels docked into a DockSpace layout. Each panel has a single, well-defined responsibility. Panels communicate through shared state objects passed by reference — they do not call each other directly.

All panel code lives in `editor/panels/`. All panels are classes with an `OnImGuiRender()` method that the `EditorApp` calls each frame.

---

## Panel Communication Pattern

Panels share state through a small number of shared state objects:

```
EditorSelectionState  — which entity and tile are selected
Scene&                — the active scene (for entity/component queries)
AssetManager&         — for texture loading and previews
TilePaletteState      — which tile is selected in the palette
```

These are passed as references to panel constructors or `OnImGuiRender()` methods. Panels do not hold pointers to each other and do not call each other's methods directly.

**Rule**: If Panel A needs data that Panel B produces, that data lives in a shared state object — not in Panel B.

---

## MainMenuBar

**Responsibility**: Top menu bar with File, Edit, View, and Help menus.

**Reads**: Nothing at runtime (triggers actions).

**Modifies**: Triggers events: new project, open project, save scene, open docs, etc.

**Dependencies**: EditorApp (to trigger actions).

**MVP Features**:
- File → New Scene, Open Scene, Save Scene
- File → Open Project, Recent Projects (display only)
- File → Exit
- View → Toggle panel visibility
- Help → About dialog

**Later Features**:
- Edit → Undo, Redo
- Build / Export (much later)

**What not to include yet**:
- Undo/redo (no command system in MVP)
- Export or build options

---

## SceneViewportPanel

**Responsibility**: Displays the OpenGL framebuffer as an ImGui image. Routes mouse and keyboard input to the editor camera and tile picking system when the panel is focused and hovered.

**Reads**:
- `Framebuffer` color texture ID
- `EditorCamera` view/projection matrix
- `EditorSelectionState.hoveredTile`

**Modifies**:
- `EditorCamera` position and zoom (via mouse input)
- `EditorSelectionState.hoveredTile` (tile under mouse)
- `Framebuffer` size (if panel is resized)

**Dependencies**: Framebuffer, EditorCamera, IsometricSystem, EditorSelectionState.

**MVP Features**:
- Displays scene via `ImGui::Image(framebufferTexture)`
- Camera pan (middle mouse or alt+left drag)
- Camera zoom (scroll wheel)
- Mouse hover tile highlight
- Left-click tile paint / right-click tile erase (when in tile edit mode)
- Viewport resize detection and FBO resize

**Later Features**:
- Entity selection by clicking in viewport
- Gizmo rendering via ImGuizmo
- Play/Stop mode button overlay
- Grid toggle overlay button

**What not to include yet**:
- ImGuizmo gizmos (later)
- Play/Stop mode (later)

---

## HierarchyPanel

**Responsibility**: Displays a flat list of all entities in the scene. Allows the user to select an entity. Allows creating and deleting entities.

**Reads**:
- `Scene` — iterates entities with NameComponent
- `EditorSelectionState.selectedEntity`

**Modifies**:
- `EditorSelectionState.selectedEntity` (on click)
- `Scene` — creates or destroys entities (via buttons)

**Dependencies**: Scene, EditorSelectionState.

**MVP Features**:
- List all entities by name
- Click to select an entity
- Highlight selected entity
- "Create Entity" button
- Right-click context menu: Rename, Delete
- Show entity count

**Later Features**:
- Drag-and-drop to reparent entities
- Hierarchy tree with expand/collapse
- Entity visibility toggle
- Entity locking

**What not to include yet**:
- Parent/child hierarchy (flat list only in MVP)
- Drag to reorder

---

## InspectorPanel

**Responsibility**: Displays and edits the components of the currently selected entity.

**Reads**:
- `EditorSelectionState.selectedEntity`
- All components on the selected entity

**Modifies**:
- Component data on the selected entity (transform values, sprite settings, etc.)

**Dependencies**: Scene, EditorSelectionState, AssetManager.

**MVP Features**:
- Show "No entity selected" when nothing is selected
- Display NameComponent (editable text field)
- Display TransformComponent (editable position, scale)
- Display SpriteRendererComponent (texture path, color picker)
- Display TilemapComponent (read-only size, tileset path)
- "Add Component" button (opens dropdown of available components)
- "Remove Component" button per component

**Later Features**:
- Custom per-component UI styling
- Property undo/redo integration
- Component ordering
- Script component field display

**What not to include yet**:
- Undo/redo for property changes (later)
- Custom editor component drawers (later)

---

## AssetBrowserPanel

**Responsibility**: Displays the project's `assets/` folder contents. Allows the user to browse and preview assets (textures in MVP).

**Reads**:
- Project root path
- `std::filesystem::directory_iterator` over `assets/`
- `AssetManager` for texture thumbnails

**Modifies**: Nothing (read-only in MVP). Later: drag-and-drop to scene.

**Dependencies**: AssetManager, std::filesystem.

**MVP Features**:
- File list of `assets/` directory (recursive)
- Filter by extension (show `.png`, `.jpg`)
- Thumbnail preview for image files (64×64 via ImGui::Image)
- Click to select (highlight only — no action yet)
- Refresh button (re-scans directory)
- Current path breadcrumb display

**Later Features**:
- Drag-and-drop from browser to scene viewport (assign to SpriteRenderer)
- Double-click to open asset
- Asset metadata display
- File rename, delete (later)

**What not to include yet**:
- Drag-and-drop (later)
- Asset import settings (later)
- Non-texture asset types (later)

---

## TilePalettePanel

**Responsibility**: Displays the loaded tileset as a grid of selectable tiles. The user clicks a tile to select it for painting.

**Reads**:
- Tileset texture from AssetManager
- `TilePaletteState.tilesetPath`

**Modifies**:
- `TilePaletteState.selectedTileID` (on click)

**Dependencies**: AssetManager, TilePaletteState.

**TilePaletteState**:

```cpp
struct TilePaletteState {
    std::string tilesetPath;
    int tilesetColumns  { 8 };
    int tileWidth       { 64 };
    int tileHeight      { 32 };
    int selectedTileID  { 1 };   // 0 = none / erase
};
```

**MVP Features**:
- Display tileset as a grid of clickable tiles
- Highlight the currently selected tile
- Scroll if tileset is taller than panel
- "Eraser" mode (selectedTileID = 0)

**Later Features**:
- Multi-tile selection for stamps
- Tile metadata display (tile name, ID)
- Tileset path input field

**What not to include yet**:
- Multi-tile stamps (later)
- Animated tile preview (later)
- Multiple tileset support (later)

---

## ConsolePanel

**Responsibility**: Displays log messages from spdlog inside ImGui. Supports filtering by log level.

**Reads**:
- Log messages from the custom ImGui spdlog sink

**Modifies**: Nothing in the scene. Can clear its own message buffer.

**Dependencies**: spdlog (via custom sink).

**MVP Features**:
- Display log messages in a scrollable list
- Color-coded by level (DEBUG=grey, INFO=white, WARN=yellow, ERROR=red)
- Auto-scroll to latest message
- Clear button
- Filter by log level (toggle buttons)
- Filter by text (search box)

**Later Features**:
- Filter by log category (Engine, Renderer, Scene, etc.)
- Copy selected message to clipboard
- Export log to file

**What not to include yet**:
- Log file export (later)
- Clickable source file links (later)

---

## Panel Implementation Pattern

Each panel follows this structure:

```cpp
// editor/panels/SomePanel.h
class SomePanel {
public:
    SomePanel(/* dependencies by reference or pointer */);
    
    void OnImGuiRender();
    
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool v) { m_visible = v; }

private:
    bool m_visible { true };
    // panel-specific state
};
```

```cpp
// editor/panels/SomePanel.cpp
void SomePanel::OnImGuiRender() {
    if (!m_visible) return;
    
    if (ImGui::Begin("Panel Name", &m_visible)) {
        // panel content
    }
    ImGui::End();
}
```

The `EditorApp` calls each panel's `OnImGuiRender()` between `ImGui::NewFrame()` and `ImGui::Render()`.

---

## DockSpace Setup

The main DockSpace is created by `EditorApp` before any panels render:

```
function EditorApp::RenderImGui():
    ImGui::NewFrame()
    
    // Full-window DockSpace
    SetupDockSpace()
    
    mainMenuBar.OnImGuiRender()
    sceneViewportPanel.OnImGuiRender(framebuffer)
    hierarchyPanel.OnImGuiRender(scene, selectionState)
    inspectorPanel.OnImGuiRender(scene, selectionState, assetManager)
    assetBrowserPanel.OnImGuiRender(assetManager)
    tilePalettePanel.OnImGuiRender(assetManager, paletteState)
    consolePanel.OnImGuiRender()
    
    ImGui::Render()
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData())
```

The DockSpace layout persists to `imgui.ini` automatically by Dear ImGui.
