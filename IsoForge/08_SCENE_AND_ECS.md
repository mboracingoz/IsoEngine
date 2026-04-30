# 08 — Scene and ECS

---

## Overview

The scene system uses [EnTT](https://github.com/skypjack/entt) as the entity-component storage backend. The engine wraps EnTT in a thin `Scene` class and an `Entity` helper to provide a cleaner interface and shield higher-level code from direct registry manipulation.

---

## Core Design

```
Scene
├── entt::registry  (owns all entities and component storage)
├── Entity wrapper  (a handle: entt::entity + pointer to registry)
└── Systems         (plain functions or methods that iterate over component views)
```

The `Scene` class owns the `entt::registry`. All entity creation, destruction, and component access must go through the `Scene` or `Entity` interface. Direct registry calls outside of these classes are discouraged.

---

## Scene Class

```cpp
// engine/scene/Scene.h (pseudocode)

class Scene {
public:
    Entity CreateEntity(const std::string& name = "Entity");
    void   DestroyEntity(Entity entity);
    
    void OnUpdate(float deltaTime);
    void OnRender(Renderer& renderer, const Camera& camera);
    
    // Iteration helpers
    template<typename... Components>
    auto View() { return m_registry.view<Components...>(); }

private:
    entt::registry m_registry;
};
```

The `Scene` does not call ImGui. It does not know about the editor. It does not store editor selection state.

---

## Entity Wrapper

`Entity` is a lightweight handle. It wraps an `entt::entity` and a non-owning pointer to the registry.

```cpp
// engine/scene/Entity.h (pseudocode)

class Entity {
public:
    Entity() = default;
    Entity(entt::entity handle, entt::registry* registry);
    
    bool IsValid() const;
    
    template<typename T, typename... Args>
    T& AddComponent(Args&&... args);
    
    template<typename T>
    T& GetComponent();
    
    template<typename T>
    bool HasComponent() const;
    
    template<typename T>
    void RemoveComponent();
    
    entt::entity GetHandle() const;

private:
    entt::entity      m_handle   { entt::null };
    entt::registry*   m_registry { nullptr };
};
```

---

## Component Storage

Components are plain structs. They contain data, not behavior. All logic lives in systems.

Components are stored directly in the `entt::registry` via `registry.emplace<ComponentType>(entity)`.

There is no base class for components. EnTT does not require inheritance.

---

## Required MVP Components

### NameComponent

```cpp
struct NameComponent {
    std::string name;
};
```

Every entity should have a NameComponent. It is added automatically when `Scene::CreateEntity(name)` is called.

---

### TransformComponent

```cpp
struct TransformComponent {
    glm::vec2 position  { 0.0f, 0.0f };  // world position (or grid position in iso)
    glm::vec2 scale     { 1.0f, 1.0f };
    float     rotation  { 0.0f };         // degrees, for later use
};
```

All entities with a visual representation should have a TransformComponent.

---

### SpriteRendererComponent

```cpp
struct SpriteRendererComponent {
    std::shared_ptr<Texture> texture;   // may be null (uses missing texture)
    glm::vec4                color { 1.0f, 1.0f, 1.0f, 1.0f };  // tint
    std::string              texturePath;  // relative asset path for serialization
};
```

The `texturePath` is stored for serialization. The `texture` handle is resolved at runtime by the AssetManager.

---

### CameraComponent

```cpp
struct CameraComponent {
    float orthoSize    { 5.0f };   // half-height in world units
    bool  isPrimary    { true };   // which camera renders the game view
};
```

In the MVP editor, the game camera (CameraComponent) is different from the EditorCamera. The editor uses EditorCamera exclusively.

---

### TilemapComponent

```cpp
struct TilemapComponent {
    int width  { 20 };
    int height { 20 };
    
    // tiles[layer][y * width + x] = tile ID (0 = empty)
    std::vector<std::vector<int>> layers;
    
    std::string tilesetPath;  // path to tileset texture
    int tileWidth  { 64 };
    int tileHeight { 32 };
    int tilesetColumns { 8 };  // how many tiles per row in the tileset texture
};
```

The tilemap stores tile IDs per layer. Tile ID 0 means empty. The tileset texture is loaded by AssetManager using `tilesetPath`.

---

## Optional Later Components (Not in MVP)

These are planned but must not be implemented until the appropriate phase:

- `ScriptComponent` — Lua script reference (Later, not MVP)
- `AudioSourceComponent` — audio clip and playback settings (Later, not MVP)
- `Rigidbody2DComponent` — Box2D body handle (Later, not MVP)
- `AnimationComponent` — sprite animation state (Later, not MVP)

---

## Entity Lifecycle Pseudocode

### CreateEntity

```
function Scene::CreateEntity(name) -> Entity:
    handle = m_registry.create()
    entity = Entity(handle, &m_registry)
    
    entity.AddComponent<NameComponent>(name)
    entity.AddComponent<TransformComponent>()  // default values
    
    return entity
```

### DestroyEntity

```
function Scene::DestroyEntity(entity):
    if entity.IsValid():
        m_registry.destroy(entity.GetHandle())
```

### AddComponent

```
function Entity::AddComponent<T>(args...) -> T&:
    assert(not HasComponent<T>())
    return m_registry->emplace<T>(m_handle, args...)
```

### GetComponent

```
function Entity::GetComponent<T>() -> T&:
    assert(HasComponent<T>())
    return m_registry->get<T>(m_handle)
```

---

## Scene Update Flow

```
function Scene::OnUpdate(deltaTime):
    // In MVP, no physics or scripting systems
    // Camera system (later: update active camera)
    // Script system (later: call Lua update)
    // Animation system (later: advance animation frames)
    pass  // nothing to update in early MVP
```

---

## Scene Render Flow

```
function Scene::OnRender(renderer, camera):
    // 1. Collect and depth-sort tilemap tiles
    view = m_registry.view<TransformComponent, TilemapComponent>()
    for entity in view:
        tm = entity.get<TilemapComponent>()
        renderer.RenderTilemap(tm, camera)
    
    // 2. Collect and depth-sort sprites
    sprites = []
    view = m_registry.view<TransformComponent, SpriteRendererComponent>()
    for entity in view:
        tr = entity.get<TransformComponent>()
        sr = entity.get<SpriteRendererComponent>()
        sprites.push_back({ isoDepth(tr.position), tr, sr })
    
    sort sprites by isoDepth ascending
    
    for sprite in sprites:
        renderer.RenderSprite(sprite.tr.position, sprite.sr.texture, sprite.sr.color)
```

---

## Editor Selection

Entity selection is an editor concern. The engine `Scene` does not track which entity is selected.

Selection state lives in `editor/EditorSelectionState.h`:

```cpp
// editor/EditorSelectionState.h
struct EditorSelectionState {
    Entity selectedEntity;   // may be invalid (nothing selected)
    ivec2  selectedTile { -1, -1 };  // may be (-1,-1) for nothing selected
};
```

The HierarchyPanel writes to `selectedEntity`. The InspectorPanel reads from `selectedEntity`. The SceneViewportPanel reads `selectedTile` and `selectedEntity` for highlight rendering.

---

## Hierarchy Support

The MVP hierarchy is flat — no parent/child entity relationships. All entities are top-level.

Parent/child relationships are a future feature (Later, not MVP).

In the HierarchyPanel, all entities with a `NameComponent` are displayed in a flat list. The panel iterates:

```
view = scene.View<NameComponent>()
for entity in view:
    display entity name
    if clicked: selectionState.selectedEntity = entity
```

---

## System Rules

- Systems are functions or methods, not classes with inheritance.
- Systems must not create or destroy entities (use a deferred destroy list if needed to avoid iterator invalidation).
- Systems in `engine/scene/` must not call ImGui.
- Systems in `editor/` may query scene data and call ImGui to display it.
