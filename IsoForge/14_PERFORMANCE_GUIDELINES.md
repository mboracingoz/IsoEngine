# 14 — Performance Guidelines

Hardware reference: Intel i3-6100, GTX 750 Ti (2 GB VRAM), 8 GB RAM, CachyOS Linux.

These guidelines exist to keep the editor responsive and the build fast on this hardware. They are practical rules, not theoretical optimizations.

---

## General Rule

**Make it correct first. Make it fast only when there is an actual problem.**

Do not optimize prematurely. Profile before optimizing. The GTX 750 Ti is more than capable of rendering thousands of 2D tiles efficiently. The i3-6100 can drive a Dear ImGui editor without stress. The bottleneck in early development is developer productivity, not CPU/GPU throughput.

---

## Editor Performance Rules

### Rule E1: Keep panels lightweight

Each panel's `OnImGuiRender()` is called every frame (~60 times per second). Panel code must not do expensive work per frame.

- Do not scan the filesystem every frame. Cache directory listings and refresh only on user request or when the directory changes.
- Do not load textures inside `OnImGuiRender()`. Load once, cache, display the cached handle.
- Do not sort large arrays every frame. Sort only when the data changes.

### Rule E2: No dynamic allocations in hot paths

Avoid `std::vector::push_back` inside `OnImGuiRender()` or the render loop without pre-allocated capacity.

Use pre-allocated buffers or pre-cleared vectors that are refilled each frame without reallocation.

### Rule E3: Avoid string formatting per frame for unchanged data

Formatting entity names, file paths, and component values into strings per frame wastes CPU. Cache formatted strings and regenerate only when the underlying data changes.

### Rule E4: Cap the console panel's message buffer

The console panel can accumulate thousands of log messages during a long session. Cap the buffer at a reasonable size (e.g., 1000 messages). Discard old messages when the buffer is full (circular or trim-from-front).

---

## Renderer Performance Rules

### Rule R1: Use simple draw calls first

In early phases, drawing each tile individually with a separate draw call is acceptable. The GTX 750 Ti supports well over 1000 draw calls per frame at 60 fps for 2D content without issue.

Batch rendering (combining many quads into one draw call) is a later optimization. Do not design a batching system before the basic renderer works correctly.

### Rule R2: Only resize the framebuffer when the viewport size actually changes

Framebuffer resize involves GPU texture reallocation. Triggering it every frame at constant viewport size wastes time and can cause visual glitches.

Compare the new size to the current size and only resize when they differ by more than 1 pixel.

### Rule R3: Avoid clearing the framebuffer unnecessarily

Clear the framebuffer's color and depth buffers once at the start of each frame's scene render pass. Do not clear mid-frame unless explicitly required.

### Rule R4: Do not use mipmaps for editor tile textures in MVP

Mipmap generation (via `glGenerateMipmap()`) takes time and memory. For a 2D tile editor where tiles are displayed at or near their native resolution, mipmaps provide little benefit. Use `GL_NEAREST` or `GL_LINEAR` filtering without mipmaps in MVP.

---

## Texture Memory Rules

### Rule T1: Do not load all project textures at startup

Load textures only when they are needed (first use). Lazy loading avoids filling VRAM with textures that are never seen in the current editing session.

### Rule T2: Keep tileset textures reasonable in size

On a GTX 750 Ti with 2 GB VRAM, very large texture atlases (4096×4096 RGBA) consume 64 MB of VRAM each. For the MVP, keep tileset textures at 512×512 or 1024×1024. Multiple moderate-sized textures are easier to manage than one giant atlas.

### Rule T3: Prefer RGBA8 for simplicity

Use 32-bit RGBA (`GL_RGBA8`) for all textures in the MVP. Do not use compressed texture formats in the MVP — the added complexity is not worth the VRAM savings at this scale.

---

## Build Time Rules

### Rule B1: Keep headers small

Headers that are included by many translation units amplify compile time. Move implementation details to `.cpp` files. Forward-declare instead of including in headers where possible.

### Rule B2: Do not put large template instantiations in headers

Templates instantiated in multiple translation units increase compile time and object file size. If a template is only used in one place, define and instantiate it in the corresponding `.cpp` file.

### Rule B3: Avoid rebuilding third-party libraries

Third-party libraries (spdlog, SDL3, ImGui) are compiled once when built for the first time and cached by CMake. Do not modify third-party headers or trigger their rebuild unnecessarily.

### Rule B4: Use Ninja as the CMake generator

Ninja is faster than GNU Make for incremental builds, especially on multi-core systems.

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

---

## ImGui Performance Rules

### Rule I1: Do not create new ImGui windows every frame for static data

Use `ImGui::Begin()` / `ImGui::End()` pairs that persist across frames. Do not dynamically create and destroy ImGui windows.

### Rule I2: Use `ImGuiListClipper` for large lists

If the hierarchy panel or asset browser must display hundreds of items, use `ImGuiListClipper` to render only the visible items. Without it, ImGui processes all list items even if only a few are visible.

### Rule I3: Avoid per-frame text formatting for stable labels

Use `ImGui::Text("Position")` not `ImGui::Text(std::format("Position: {}", ...))` for labels that do not change. For values that do change, format once per frame with a stack buffer.

---

## Tilemap Rendering Rules

### Rule TM1: Render in depth order

Render tiles in isometric depth order (increasing `gridX + gridY`). This ensures correct visual overlap without relying on the depth buffer.

### Rule TM2: Skip empty tiles

Tile ID 0 means empty. Do not issue draw calls for empty tiles. Skip them in the render loop:

```
if tileID == 0: continue
```

### Rule TM3: Cull off-screen tiles

In the tile render loop, check if a tile's screen position is outside the viewport bounds. If so, skip it. This provides a simple but effective culling step without a spatial data structure.

```
screenPos = GridToScreen(x, y, camera)
if screenPos.x < -TILE_WIDTH  or screenPos.x > viewportWidth  + TILE_WIDTH:  continue
if screenPos.y < -TILE_HEIGHT or screenPos.y > viewportHeight + TILE_HEIGHT: continue
```

### Rule TM4: Batch later, not now

Sprite batching (combining many quads into one draw call with a shared vertex buffer) is the correct optimization for tilemap rendering at scale. Implement it only after the basic per-tile draw loop works correctly and performance is measured.

---

## Asset Loading Rules

### Rule AL1: Load textures once, cache always

Never load the same texture twice. Always check the cache before loading.

### Rule AL2: Free CPU pixel data immediately after GPU upload

After calling `glTexImage2D()`, free the stb_image CPU pixel data with `stbi_image_free()`. Do not retain CPU copies of texture data after GPU upload.

### Rule AL3: Do not scan directories at runtime in the render loop

Asset browser directory scanning must happen only on user request (Refresh button) or on startup. Never scan the filesystem inside a per-frame function.

---

## Logging Performance Rules

### Rule L1: Disable TRACE level in development builds

spdlog's `TRACE` level generates a log call on every occurrence. In a frame loop, this can produce millions of log entries per minute. Disable `TRACE` at compile time:

```cmake
target_compile_definitions(IsoForgeEngine PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
```

### Rule L2: Do not log inside the render loop for normal operation

Logging inside the tile render loop, the input handler, or the framebuffer bind/unbind will generate enormous log output and slow down the editor significantly. Log per-frame events only when debugging a specific issue, behind a `#ifdef` or a debug flag.

### Rule L3: Cap the ImGui console panel buffer

See Rule E4. Large console buffers slow down the console panel's `OnImGuiRender()` as it iterates all messages. Cap at 500–1000 messages.
