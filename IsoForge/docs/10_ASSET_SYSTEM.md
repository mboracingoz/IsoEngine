# 10 — Asset System

---

## Overview

The asset system in the MVP is intentionally simple. It loads textures from disk, caches them by path, and provides a missing-texture fallback. There is no complex import pipeline, no asset database, and no background loading in the MVP.

The principle: load what is needed, when it is needed, cache it, and provide a safe fallback if something is missing.

---

## Asset Root Folder

All project assets live under the `sandbox_project/assets/` folder (or equivalent for a loaded project). All asset paths are stored as relative paths from the project root.

```
sandbox_project/
└── assets/
    └── textures/
        ├── tileset.png
        ├── player.png
        └── missing.png   ← the missing texture fallback
```

**Rule**: Asset paths are always stored relative to the project root. Absolute paths are never stored in JSON files or component data.

---

## Relative Path Handling

When loading an asset, the AssetManager constructs the full path:

```
fullPath = projectRootPath / relativePath
```

Where `projectRootPath` is set when the project is loaded and `relativePath` comes from the component or serialized data.

`std::filesystem::path` is used for path construction to handle OS differences cleanly.

---

## Texture Loading

Textures are loaded using `stb_image`. The process:

1. Read the file from disk using `stb_image_load()`.
2. Upload the pixel data to the GPU as an OpenGL texture (`glTexImage2D`).
3. Store the OpenGL texture handle and metadata in a `Texture` object.
4. Free the CPU-side pixel data (`stb_image_free()`).
5. Return the `Texture` (RAII — destructor calls `glDeleteTextures`).

---

## Texture Caching

The `AssetManager` keeps a `std::unordered_map<std::string, std::shared_ptr<Texture>>` as a cache.

```
function AssetManager::LoadTexture(relativePath) -> shared_ptr<Texture>:
    fullPath = BuildFullPath(relativePath)
    
    if cache.contains(fullPath):
        return cache[fullPath]
    
    texture = LoadTextureFromDisk(fullPath)
    
    if texture is null:
        LOG_ASSET_WARN("Failed to load texture: {}", fullPath)
        return GetMissingTexture()
    
    cache[fullPath] = texture
    return texture
```

The cache uses `std::shared_ptr<Texture>` so that multiple components referencing the same texture share one GPU texture object.

---

## Missing Texture Fallback

A 4×4 magenta/black checkerboard texture is generated programmatically at startup and stored in the AssetManager.

```
function AssetManager::CreateMissingTexture() -> shared_ptr<Texture>:
    // 4x4 checkerboard: magenta (255, 0, 255) and black (0, 0, 0)
    pixels = [
        255, 0, 255, 255,    0,   0,   0, 255,   255, 0, 255, 255,    0,   0,   0, 255,
          0, 0,   0, 255,  255,   0, 255, 255,     0,  0,   0, 255,  255,   0, 255, 255,
        255, 0, 255, 255,    0,   0,   0, 255,   255, 0, 255, 255,    0,   0,   0, 255,
          0, 0,   0, 255,  255,   0, 255, 255,     0,  0,   0, 255,  255,   0, 255, 255,
    ]  // 4x4 RGBA
    
    texture = UploadToGPU(pixels, 4, 4)
    return texture
```

The missing texture is always available, created at startup, and never evicted from cache.

---

## Unloading Unused Textures

In MVP, textures are not automatically unloaded. The `AssetManager` holds `shared_ptr` references. When the scene is unloaded, the component `shared_ptr` references drop. If no other reference exists, the texture is deallocated automatically via the `shared_ptr` destructor.

A manual `UnloadUnused()` function can be called to evict cache entries with reference count == 1 (only the cache holds a reference):

```
function AssetManager::UnloadUnused():
    for entry in cache:
        if entry.second.use_count() == 1:
            LOG_ASSET_DEBUG("Unloading unused texture: {}", entry.first)
            cache.erase(entry.first)
```

This is called when the project is closed or when the user explicitly triggers a cleanup.

---

## Asset Browser Integration

The `AssetBrowserPanel` in the editor reads the project's `assets/` folder using `std::filesystem::directory_iterator` and displays a flat file browser.

```
function AssetBrowserPanel::Refresh():
    entries.clear()
    for path in filesystem::recursive_directory_iterator(assetRootPath):
        if path.extension() is ".png" or ".jpg":
            entries.push_back(path)
```

For texture files, the panel can load the currently selected texture using `AssetManager::LoadTexture()` and render it as an `ImGui::Image()`. The preview uses the same cache as the rest of the editor, so there is no double-loading.

The AssetBrowserPanel is read-only in MVP. Dragging assets onto the scene viewport or inspector is a later feature.

---

## Pseudocode

### LoadTexture

```
function AssetManager::LoadTexture(relativePath) -> shared_ptr<Texture>:
    fullPath = m_projectRoot / relativePath
    key = fullPath.string()
    
    if m_cache.count(key):
        return m_cache[key]
    
    int w, h, channels
    unsigned char* data = stbi_load(key.c_str(), &w, &h, &channels, 4)
    
    if data is null:
        LOG_ASSET_ERROR("Could not load: {}", key)
        return m_missingTexture
    
    texture = make_shared<Texture>()
    texture.Upload(data, w, h)
    stbi_image_free(data)
    
    m_cache[key] = texture
    LOG_ASSET_INFO("Loaded texture: {} ({}x{})", key, w, h)
    return texture
```

### GetTexture

```
function AssetManager::GetTexture(relativePath) -> shared_ptr<Texture>:
    // Same as LoadTexture — loads on first access
    return LoadTexture(relativePath)
```

### UnloadUnused

```
function AssetManager::UnloadUnused():
    to_remove = []
    for [key, tex] in m_cache:
        if tex.use_count() == 1:
            to_remove.push_back(key)
    for key in to_remove:
        m_cache.erase(key)
        LOG_ASSET_DEBUG("Evicted: {}", key)
```

### DrawAssetBrowserTexturePreview

```
function AssetBrowserPanel::DrawTexturePreview(relativePath):
    texture = assetManager.LoadTexture(relativePath)
    if texture:
        size = ImVec2(64, 64)  // thumbnail size
        ImGui::Image(texture.GetImTextureID(), size)
    else:
        ImGui::Text("[missing]")
```

---

## Later: Asset Database (Not MVP)

A future asset database would:
- Assign a unique UUID to each asset.
- Store import settings (compression, filtering, max size).
- Track asset dependencies.
- Support reimporting after changes on disk.
- Enable asset references by UUID instead of path (more robust when files are moved).

This is explicitly **not part of MVP**. The path-based system is sufficient for the MVP scope and can be replaced later.

---

## Phase 4A Note

Phase 4A introduces the basic texture asset foundation only:
- `Texture2D` as an engine-side OpenGL texture RAII wrapper
- `AssetManager` as an engine-side texture loader/cache keyed by relative asset path
- A generated 4x4 magenta/black missing-texture fallback
- `AssetBrowserPanel` listing files under `sandbox_project/assets/textures/`

Still deferred in Phase 4A:
- Texture tile rendering
- Tileset atlas slicing
- Asset thumbnails in the browser
- Drag/drop asset workflows

## Phase 4B Note

Phase 4B adds a selected-texture preview to `AssetBrowserPanel`:
- Clicking a listed texture loads it through engine-side `AssetManager`
- The selected texture name, relative path, and size are shown in the panel
- The preview is rendered editor-side with `ImGui::Image()` using `Texture2D::GetRendererID()`
- Invalid or failed texture loads fall back to the generated missing texture

Still deferred after Phase 4B:
- Texture tile rendering in the scene viewport
- Tileset atlas slicing
- Full-browser thumbnails for every asset
- Drag/drop asset workflows
