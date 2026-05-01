# 06 — Rendering and Viewport

---

## Overview

The rendering pipeline in IsoForge Editor has two distinct parts:

1. **Scene rendering**: The engine's Renderer draws the scene (tiles, sprites, grid) into an OpenGL Framebuffer Object (FBO).
2. **Editor display**: The editor's SceneViewportPanel displays the FBO's color texture inside a Dear ImGui image widget.

These two parts must never be mixed. The renderer does not know about ImGui. ImGui does not make OpenGL draw calls for scene content.

---

## SDL3 Window and OpenGL Context

The window and OpenGL context are created once at startup by the platform layer.

**Pseudocode — Initialization**:

```
function InitPlatform():
    SDL_Init(SDL_INIT_VIDEO)
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)
    
    window = SDL_CreateWindow("IsoForge Editor", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)
    glContext = SDL_GL_CreateContext(window)
    
    SDL_GL_MakeCurrent(window, glContext)
    SDL_GL_SetSwapInterval(1)  // vsync
    
    gladLoadGL()  // or equivalent GL loader
    
    return { window, glContext }
```

**Important**: The OpenGL context must be created before any OpenGL calls or ImGui OpenGL backend initialization.

---

## Renderer Lifecycle

```
Startup:
  InitPlatform()      → SDL3 window + GL context
  InitImGui()         → ImGui context, SDL3 backend, OpenGL3 backend
  InitRenderer()      → compile shaders, create VAOs, create framebuffer
  InitScene()         → create scene, load initial data

Per Frame:
  PollEvents()        → SDL3 events → ImGui + input handler
  UpdateEditor()      → update panels, process commands
  RenderSceneToFBO()  → bind FBO, draw scene, unbind FBO
  RenderImGui()       → draw all panels (includes Image(fbo_texture))
  SwapBuffers()       → SDL_GL_SwapWindow()

Shutdown:
  ShutdownScene()
  ShutdownRenderer()
  ShutdownImGui()
  ShutdownPlatform()
```

---

## Framebuffer Design

The scene is rendered into an FBO (Framebuffer Object), not directly into the default OpenGL framebuffer (the window).

This is required because:
- Dear ImGui occupies the default framebuffer.
- The scene image must be passed to ImGui as a texture, not drawn directly.
- The viewport panel can be resized independently of the window.

### FBO Structure

```
Framebuffer:
  - Color attachment: GL_RGBA8 texture (scene image)
  - Depth attachment: GL_DEPTH24_STENCIL8 renderbuffer (depth testing for depth sorting later)
```

**Pseudocode — Framebuffer**:

```
struct Framebuffer:
    fbo_id:    GLuint
    color_id:  GLuint  // texture ID passed to ImGui::Image()
    depth_id:  GLuint  // renderbuffer
    width:     int
    height:    int

function Framebuffer::Create(width, height):
    glGenFramebuffers(1, &fbo_id)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id)
    
    // Color attachment
    glGenTextures(1, &color_id)
    glBindTexture(GL_TEXTURE_2D, color_id)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_id, 0)
    
    // Depth attachment
    glGenRenderbuffers(1, &depth_id)
    glBindRenderbuffer(GL_RENDERBUFFER, depth_id)
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height)
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_id)
    
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    glBindFramebuffer(GL_FRAMEBUFFER, 0)

function Framebuffer::Resize(new_width, new_height):
    Destroy()
    Create(new_width, new_height)

function Framebuffer::Bind():
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id)
    glViewport(0, 0, width, height)

function Framebuffer::Unbind():
    glBindFramebuffer(GL_FRAMEBUFFER, 0)

function Framebuffer::GetColorTextureID() -> ImTextureID:
    return (ImTextureID)(uintptr_t)color_id
```

---

## Rendering the Scene to the Framebuffer

**Pseudocode — Scene render pass**:

```
function RenderSceneToFBO(framebuffer, scene, camera):
    framebuffer.Bind()
    
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f)  // dark background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    
    renderer.BeginScene(camera.GetViewProjectionMatrix())
    
    // Render tilemap layers (bottom to top)
    scene.ForEachTilemapComponent([](TilemapComponent& tm):
        renderer.RenderTilemap(tm)
    )
    
    // Render sprite entities
    scene.ForEachSpriteComponent([](TransformComponent& tr, SpriteRendererComponent& sr):
        renderer.RenderSprite(tr.position, sr.texture, sr.color)
    )
    
    // Render isometric grid overlay (editor-only — passed as parameter)
    if showGrid:
        isoGrid.Render(camera)
    
    renderer.EndScene()
    
    framebuffer.Unbind()
```

---

## Displaying the Framebuffer in ImGui

**Pseudocode — SceneViewportPanel**:

```
function SceneViewportPanel::OnImGuiRender(framebuffer):
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0))
    ImGui::Begin("Scene Viewport")
    
    // Detect viewport size
    viewportSize = ImGui::GetContentRegionAvail()
    
    // Resize framebuffer if panel was resized
    if viewportSize != framebuffer.GetSize():
        framebuffer.Resize(viewportSize.x, viewportSize.y)
        camera.SetViewportSize(viewportSize.x, viewportSize.y)
    
    // Draw the scene texture
    textureID = framebuffer.GetColorTextureID()
    ImGui::Image(textureID, viewportSize, ImVec2(0, 1), ImVec2(1, 0))
    // Note: UV flip (0,1)→(1,0) corrects OpenGL's bottom-left origin
    
    // Check if mouse is inside the viewport for input routing
    viewportHovered = ImGui::IsItemHovered()   // after ImGui::Image()
    viewportFocused = ImGui::IsWindowFocused()
    
    ImGui::End()
    ImGui::PopStyleVar()
```

---

## Viewport Resizing

When the user resizes the SceneViewportPanel:
1. Detect the new size in `ImGui::GetContentRegionAvail()`.
2. Compare to the current framebuffer size.
3. If different, call `Framebuffer::Resize(newW, newH)`.
4. Update the camera's projection matrix to match the new aspect ratio.

**Common mistake**: Resizing the framebuffer every frame regardless of whether the size changed. This is expensive and causes a full GPU texture reallocation on every frame. Only resize when the size actually changes.

---

## Camera Handling

The editor uses an **EditorCamera** that is separate from any in-game camera.

The EditorCamera is responsible for:
- Maintaining a view matrix (position, zoom).
- Computing an orthographic projection matrix for 2D.
- Handling pan (middle mouse drag or alt + left mouse drag).
- Handling zoom (scroll wheel).

```
function EditorCamera::GetViewProjectionMatrix() -> mat4:
    view = translate(identity, -position)
    projection = ortho(
        -viewportWidth  / 2.0f / zoom,
         viewportWidth  / 2.0f / zoom,
        -viewportHeight / 2.0f / zoom,
         viewportHeight / 2.0f / zoom,
        -1.0f, 1.0f
    )
    return projection * view
```

The EditorCamera lives in `editor/EditorCamera.h`. It is **not** in `engine/`. It is an editor concern.

---

## Input Routing

Dear ImGui consumes some input events. The scene viewport must only receive input when:
- The viewport panel is hovered (`ImGui::IsWindowHovered()`).
- ImGui is not consuming mouse input (`!ImGui::GetIO().WantCaptureMouse`).

**Rule**:
- If ImGui wants the mouse (user is interacting with a panel widget), do not pass mouse input to the scene camera.
- If the mouse is over the scene viewport and ImGui does not want it, pass input to the editor camera and tile picker.

**Phase 2C note**:
- The Scene Viewport camera may be reset with `R` while the viewport image is hovered or the Scene Viewport window is focused.
- This reset shortcut must not trigger while ImGui text input is active.

```
function EditorApp::UpdateInput():
    io = ImGui::GetIO()
    
    if viewportHovered and not io.WantCaptureMouse:
        editorCamera.ProcessMouseInput(mousePos, mouseButtons, scrollDelta)
        isoSystem.UpdateHoveredTile(mousePos, editorCamera)
    
    if not io.WantCaptureKeyboard:
        ProcessEditorKeyboardShortcuts()
```

---

## Common Mistakes

### Rendering directly into the editor window

If you call OpenGL draw calls without binding the FBO first, you draw directly over the ImGui window. The result is a corrupted display. Always bind the FBO before any scene draw call.

### Not handling viewport resize

If the framebuffer is not resized when the panel is resized, the scene image will be stretched or clipped. Always compare panel size to FBO size each frame.

### Mixing editor UI draw with scene rendering

Do not call `ImGui::*` functions between `Framebuffer::Bind()` and `Framebuffer::Unbind()`. ImGui renders to the default framebuffer, not the FBO. Keep scene rendering and ImGui rendering in separate phases of the frame.

### Not separating editor camera from game camera

The EditorCamera should not exist in `engine/`. If you put it in the engine, you couple the engine to editor-only behavior. The engine scene can have its own `CameraComponent` for the game's camera. These are different objects with different purposes.

### UV flip forgotten

OpenGL textures have their origin at the bottom-left, but ImGui images have their origin at the top-left. Pass `ImVec2(0, 1)` and `ImVec2(1, 0)` as the UV coordinates to `ImGui::Image()` to flip the image correctly.
