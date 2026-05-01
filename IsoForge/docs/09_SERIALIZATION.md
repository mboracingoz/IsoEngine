# 09 — Serialization

---

## Overview

Serialization in IsoForge Editor uses [nlohmann/json](https://github.com/nlohmann/json) for all file-level persistence. JSON is human-readable, easy to debug, and sufficient for the small file sizes this project handles in MVP.

Serialization is strictly file-level. It is not used for runtime communication, networking, or real-time data. It handles only loading from and saving to disk.

---

## What Is Serialized

| Data | Format | Location |
|---|---|---|
| Project metadata | `project.json` | `sandbox_project/project.json` |
| Scene (entities, components) | `scene.json` | `sandbox_project/scenes/*.json` |
| Tilemap layers and tile data | `tilemap.json` | `sandbox_project/tilemaps/*.json` |
| Editor settings (later) | `editor_settings.json` | User config dir or project root |
| Asset metadata (later) | `*.meta` files | Next to each asset file |

---

## What Is NOT Serialized

- Runtime state (which entity is currently selected, editor camera position between sessions in MVP)
- OpenGL handles (texture IDs, VAO IDs — these are GPU-side and rebuilt on load)
- The `entt::registry` directly (EnTT entities are not serialized as raw handles)
- Temporary editor state (console log history, panel visibility in MVP)

---

## Runtime vs Editor Data Separation

Scene files contain **runtime data only** (entities, components, transform values, tilemap references). They do not contain editor selection state, panel layouts, or debug flags.

Editor settings (panel layout, preferences) are serialized separately into a different file that is not part of the project data. This ensures that sharing a project folder does not share personal editor preferences.

---

## Versioning Strategy

Every JSON file includes a `"version"` field at the top level. This is a simple integer.

```json
{
  "version": 1,
  ...
}
```

In MVP, the version is always 1 and no migration is required. When the format changes in the future, the version is incremented and a migration step is added to the deserializer.

Missing version field: treat as version 1 and log a warning.

---

## Missing Field Fallback

If a JSON field is missing during deserialization:
1. Log a warning with the field name and the file path.
2. Use the component's default value.
3. Do not crash.

Example:

```cpp
// Reading a TransformComponent
float posX = json.value("posX", 0.0f);  // default 0.0 if missing
float posY = json.value("posY", 0.0f);
```

This makes JSON files forward-compatible when new fields are added in later versions.

---

## Error Handling

If a file does not exist or cannot be parsed:
1. Log an error with the file path and what was attempted.
2. Return a failure result (do not throw exceptions across module boundaries).
3. The caller decides how to handle the failure (show a dialog, use defaults, etc.).

Use `try/catch` around `nlohmann::json::parse()` calls since the library throws on malformed JSON.

---

## Logging Serialization Events

All serialization operations should log at appropriate levels:

| Event | Level |
|---|---|
| File load started | DEBUG |
| File load succeeded | INFO |
| Missing optional field | WARN |
| File not found | ERROR |
| Parse error | ERROR |
| File save succeeded | INFO |
| File save failed | ERROR |

Use the `Serialization` log category.

---

## Example JSON Structures

### project.json

```json
{
  "version": 1,
  "name": "Sandbox Project",
  "startScene": "scenes/main_scene.json",
  "tileWidth": 64,
  "tileHeight": 32
}
```

Fields:
- `version` — file format version
- `name` — display name of the project
- `startScene` — relative path to the default scene to open
- `tileWidth`, `tileHeight` — project-wide tile size defaults

---

### scene.json

```json
{
  "version": 1,
  "name": "Main Scene",
  "entities": [
    {
      "id": 1,
      "name": "Tilemap Entity",
      "components": {
        "Transform": {
          "posX": 0.0,
          "posY": 0.0,
          "scaleX": 1.0,
          "scaleY": 1.0,
          "rotation": 0.0
        },
        "Tilemap": {
          "tilemapFile": "tilemaps/test_tilemap.json"
        }
      }
    },
    {
      "id": 2,
      "name": "Player Marker",
      "components": {
        "Transform": {
          "posX": 3.0,
          "posY": 3.0,
          "scaleX": 1.0,
          "scaleY": 1.0,
          "rotation": 0.0
        },
        "SpriteRenderer": {
          "texturePath": "assets/textures/player.png",
          "colorR": 1.0,
          "colorG": 1.0,
          "colorB": 1.0,
          "colorA": 1.0
        }
      }
    }
  ]
}
```

Notes:
- Entity `"id"` is a serialization ID, not an `entt::entity` handle. New handles are created at load time.
- `"components"` is a JSON object where each key is the component type name.
- The `"Tilemap"` component stores a reference to the tilemap file, not the tile data inline (tilemap data can be large).

---

### tilemap.json

```json
{
  "version": 1,
  "name": "Test Tilemap",
  "width": 10,
  "height": 10,
  "tileWidth": 64,
  "tileHeight": 32,
  "tilesetPath": "assets/textures/tileset.png",
  "tilesetColumns": 8,
  "layers": [
    {
      "name": "Ground",
      "tiles": [
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 2, 2, 2, 1, 1, 1, 1, 1,
        1, 1, 2, 2, 2, 1, 1, 1, 1, 1,
        1, 1, 2, 2, 2, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1
      ]
    },
    {
      "name": "Objects",
      "tiles": [
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
      ]
    }
  ]
}
```

Notes:
- `tiles` is a flat array, row-major: `tile[y * width + x]`.
- Tile ID `0` means empty (do not render).
- Tile IDs are 1-indexed (tile 1 = first tile in tileset, row 0, col 0).
- The array length must equal `width × height`.
- Multiple layers are ordered bottom-to-top (index 0 = ground layer, drawn first).

---

## Serializer Design

Serializers live in `engine/serialization/`. They are stateless functions or thin classes. They do not hold references to the scene or registry after the serialize/deserialize call completes.

```
SceneSerializer::Serialize(scene, filePath)
    → writes scene.json

SceneSerializer::Deserialize(filePath, scene)
    → reads scene.json, creates entities in scene

TilemapSerializer::Serialize(tilemap, filePath)
    → writes tilemap.json

TilemapSerializer::Deserialize(filePath) → TilemapComponent
    → reads tilemap.json, returns populated TilemapComponent
```

Serializers return a bool or a result type indicating success or failure. They do not throw.

---

## Phase 3D MVP Note

Phase 3D adds a temporary tilemap-only JSON save/load path for the current debug editor workflow. This is not the final scene serialization system and does not serialize ECS state, scene hierarchy, assets, or editor layout.

Fixed MVP file path:
- `sandbox_project/tilemaps/debug_tilemap.json`

Current debug tilemap JSON shape:

```json
{
  "version": 1,
  "columns": 20,
  "rows": 20,
  "empty_tile": -1,
  "tiles": [-1, -1, 1, 2]
}
```

Notes:
- `tiles` remains a flat row-major array.
- The array length must equal `columns * rows`.
- Load/save currently applies only to `TilemapData`.
- Final scene serialization and asset-backed tile workflows remain future work.

## Phase 3E Project-Local UX Note

Phase 3E keeps tilemap JSON save/load limited to the project-local folder:
- `sandbox_project/tilemaps/`

Current editor UX:
- `File > Save Tilemap` saves to the current tilemap path if one exists.
- `File > Save Tilemap As...` opens a small ImGui modal and saves to `sandbox_project/tilemaps/<name>.json`.
- `File > Load Tilemap` opens a small ImGui modal that lists available `.json` files from `sandbox_project/tilemaps/`.

Notes:
- This is still tilemap-only save/load, not full scene serialization.
- No native file dialog is used in this phase.
- Tilemap files stay inside the project tilemaps directory for now.
