# 07 — Isometric System

---

## Overview

The isometric system converts between three coordinate spaces and provides the tile grid rendering and mouse hover detection that are central to the editor's functionality.

Understanding the three coordinate spaces is essential before writing any isometric code.

---

## Coordinate Spaces

### Grid Coordinates (Tile Space)

A logical 2D integer grid where each cell represents one isometric tile.

```
gridX = column index (integer)
gridY = row index (integer)
```

This is the "game world" coordinate. Tile data, tilemaps, and entity positions in the isometric world are expressed in grid coordinates.

### World Coordinates

A continuous 2D floating-point space in the "rendered world". This is the intermediate space between grid and screen. For a pure 2D isometric renderer, world coordinates and screen coordinates before camera transform are often the same thing.

In this implementation, world coordinates are the screen-space pixel positions *before* applying the camera offset and zoom. They represent where tiles would appear if the camera were at origin with no zoom.

### Screen Coordinates

The final pixel position on the viewport after applying the camera transform (pan offset and zoom).

```
screenPos = worldPos * zoom + cameraOffset
```

Mouse position is in screen coordinates. Before using the mouse for tile picking, it must be converted to world coordinates.

---

## Tile Size Constants

```
TILE_WIDTH  = 64   // pixels in world space (horizontal span of one tile diamond)
TILE_HEIGHT = 32   // pixels in world space (vertical span of one tile diamond)
```

These constants should be configurable per project in the future, but are fixed in MVP.

For standard isometric tiles:
- `TILE_WIDTH` is the full horizontal width of the diamond.
- `TILE_HEIGHT` is the full vertical height of the diamond.
- A sprite for one tile is typically `TILE_WIDTH × (TILE_HEIGHT + sprite_top_height)`.

---

## Origin Handling

The isometric grid origin (grid 0,0) is placed at a chosen world-space anchor point called `gridOrigin`. In MVP, this can be fixed at `(0, 0)` world space, or offset so the grid appears centered in the viewport.

```
gridOriginX = viewportWidth  / 2.0f   // center of viewport
gridOriginY = viewportHeight / 4.0f   // slightly above center
```

The origin is the screen position of grid tile (0, 0) when camera is at identity.

---

## Coordinate Conversion Formulas

### Grid to World (Screen without camera)

```
worldX = (gridX - gridY) * (TILE_WIDTH  / 2)
worldY = (gridX + gridY) * (TILE_HEIGHT / 2)
```

Adding the grid origin offset:

```
worldX += gridOriginX
worldY += gridOriginY
```

### World to Grid (Screen without camera)

```
// Remove origin offset first
localX = worldX - gridOriginX
localY = worldY - gridOriginY

gridX = (localY / (TILE_HEIGHT / 2) + localX / (TILE_WIDTH / 2)) / 2
gridY = (localY / (TILE_HEIGHT / 2) - localX / (TILE_WIDTH / 2)) / 2
```

After computing, floor to integer grid cell:

```
tileX = floor(gridX)
tileY = floor(gridY)
```

---

## Camera Transform

The editor camera applies a pan offset and zoom to world coordinates when converting to screen coordinates.

```
screenPos = worldPos * zoom + cameraOffset
```

And inversely, converting screen → world for mouse picking:

```
worldPos = (screenPos - cameraOffset) / zoom
```

Mouse hover tile detection requires converting the mouse's screen position to world position, then to grid position.

---

## Pseudocode

### GridToScreen

```
function GridToScreen(gridX, gridY, camera) -> vec2:
    worldX = (gridX - gridY) * (TILE_WIDTH  / 2.0f) + gridOriginX
    worldY = (gridX + gridY) * (TILE_HEIGHT / 2.0f) + gridOriginY
    
    screenX = worldX * camera.zoom + camera.offset.x
    screenY = worldY * camera.zoom + camera.offset.y
    
    return vec2(screenX, screenY)
```

### ScreenToGrid

```
function ScreenToGrid(screenX, screenY, camera) -> vec2:
    // Undo camera transform
    worldX = (screenX - camera.offset.x) / camera.zoom
    worldY = (screenY - camera.offset.y) / camera.zoom
    
    // Undo grid origin offset
    localX = worldX - gridOriginX
    localY = worldY - gridOriginY
    
    // Convert to fractional grid coordinates
    halfW = TILE_WIDTH  / 2.0f
    halfH = TILE_HEIGHT / 2.0f
    
    gridX = (localY / halfH + localX / halfW) / 2.0f
    gridY = (localY / halfH - localX / halfW) / 2.0f
    
    return vec2(gridX, gridY)
```

### GetHoveredTile

```
function GetHoveredTile(mouseScreenPos, camera) -> ivec2:
    gridF = ScreenToGrid(mouseScreenPos.x, mouseScreenPos.y, camera)
    return ivec2(floor(gridF.x), floor(gridF.y))
```

### RenderIsoGrid

```
function RenderIsoGrid(renderer, camera, gridWidth, gridHeight):
    for x from 0 to gridWidth:
        for y from 0 to gridHeight:
            corners = GetTileCorners(x, y, camera)
            // Draw diamond outline using 4 lines
            renderer.DrawLine(corners.top,   corners.right, gridLineColor)
            renderer.DrawLine(corners.right, corners.bottom, gridLineColor)
            renderer.DrawLine(corners.bottom, corners.left, gridLineColor)
            renderer.DrawLine(corners.left,  corners.top, gridLineColor)

function GetTileCorners(gridX, gridY, camera) -> TileCorners:
    center = GridToScreen(gridX, gridY, camera)
    halfW  = (TILE_WIDTH  / 2.0f) * camera.zoom
    halfH  = (TILE_HEIGHT / 2.0f) * camera.zoom
    
    return TileCorners {
        top:    center + vec2(  0,   -halfH ),
        right:  center + vec2( halfW,   0   ),
        bottom: center + vec2(  0,    halfH ),
        left:   center + vec2(-halfW,   0   )
    }
```

### RenderTileHighlight

```
function RenderTileHighlight(renderer, hoveredTile, camera):
    if not hoveredTile.IsValid():
        return
    
    corners = GetTileCorners(hoveredTile.x, hoveredTile.y, camera)
    
    // Fill tile with semi-transparent highlight color
    renderer.DrawFilledQuad(corners, highlightColor)  // RGBA with alpha
    
    // Optionally draw outline
    renderer.DrawLine(corners.top,    corners.right,  outlineColor)
    renderer.DrawLine(corners.right,  corners.bottom, outlineColor)
    renderer.DrawLine(corners.bottom, corners.left,   outlineColor)
    renderer.DrawLine(corners.left,   corners.top,    outlineColor)
```

---

## Depth Sorting

In isometric rendering, tiles and sprites closer to the viewer (higher `gridX + gridY` value) must be drawn after (on top of) tiles further away.

The depth sorting rule for isometric rendering:

```
drawOrder = gridX + gridY
// Higher drawOrder = drawn later = appears in front
```

For the tilemap renderer, tiles should be iterated in order of increasing `drawOrder`.

For entities (sprites), a similar sort pass is needed before rendering. Sort entities by their isometric depth value before issuing draw calls.

In MVP, depth sorting can be implemented as a simple sort before the render loop. Optimized depth sorting (via Z-buffer tricks or spatial structures) is a later improvement.

---

## Layer Support

Phase 3A note:
- `TilemapData` is introduced as engine-side storage only.
- At this stage it only stores tile IDs for a fixed-size grid; it does not imply painting, rendering filled tiles, asset loading, or serialization yet.

Phase 3B note:
- Temporary debug painting is allowed only to verify `TilemapData` mutation and visual feedback.
- In this phase, `LMB` sets the hovered tile to ID `1` and `RMB` clears it back to `EmptyTile`.
- This is not the final tile palette or textured tile workflow.

The tilemap supports multiple layers. Each layer is rendered separately, with all tiles in one layer drawn before the next layer begins.

Layer ordering:
1. **Ground layer** — base terrain tiles (drawn first, furthest back)
2. **Object layer** — objects placed on the ground (drawn on top of ground)
3. **Overlay layer** — roofs, bridges, overhanging elements (drawn last)

Within each layer, tiles are sorted by isometric depth.

Layer implementation details are covered in `08_SCENE_AND_ECS.md` and `09_SERIALIZATION.md`.

---

## Mouse Hover Tile Detection — Full Flow

```
1. Get raw mouse position in screen pixels (from SDL3 or ImGui).
2. Convert mouse screen position to viewport-local position
   (subtract the viewport panel's top-left position in the window).
3. Call ScreenToGrid(localMousePos, editorCamera).
4. Floor the result to get the integer tile coordinate.
5. Check if the tile coordinate is within the valid tilemap bounds.
6. Store the result as hoveredTile.
7. In the render pass, call RenderTileHighlight(hoveredTile).
```

**Important**: The mouse position from ImGui (`ImGui::GetMousePos()`) is in window coordinates. The viewport panel's position (`ImGui::GetWindowPos()`) must be subtracted to get the position relative to the viewport.

---

## Common Mistakes

### Forgetting the camera offset when doing mouse picking

Mouse coordinates are in screen space. Tile positions are in world space. Failing to undo the camera transform when converting mouse → grid coordinates results in tile picking that only works at camera origin.

### Using screen pixel sizes for tile constants instead of world sizes

Tile constants (`TILE_WIDTH`, `TILE_HEIGHT`) are in world space. They should not be multiplied by zoom before the camera transform step. The camera transform applies zoom uniformly.

### Integer division instead of float division in the conversion formulas

The conversion formulas produce fractional grid coordinates. Using integer division will snap the result incorrectly. Use `float` or `double` throughout the conversion and floor at the end.

### Rendering grid lines at screen pixel thickness instead of scaled thickness

When the camera is zoomed out, 1-pixel grid lines can disappear. The renderer should scale line thickness with zoom or use a minimum visible thickness.
