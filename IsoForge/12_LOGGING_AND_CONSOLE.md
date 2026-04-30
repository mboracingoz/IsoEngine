# 12 — Logging and Console

---

## Overview

Logging uses [spdlog](https://github.com/gabime/spdlog) with per-category loggers and a custom ImGui sink that forwards messages to the editor's console panel.

The logging system is initialized once at startup. All engine and editor code uses convenience macros to log without caring about the underlying sink configuration.

---

## Log Categories

Each major system has its own logger so that messages can be filtered by source.

| Category | Logger Name | Used By |
|---|---|---|
| Engine | `"Engine"` | Core utilities, general engine messages |
| Editor | `"Editor"` | EditorApp, panels, editor commands |
| Renderer | `"Renderer"` | Renderer, framebuffer, shaders |
| Scene | `"Scene"` | Scene, ECS operations |
| Asset | `"Asset"` | AssetManager, texture loading |
| Serialization | `"Serialization"` | Scene/tilemap serializers |
| Iso | `"Iso"` | Isometric math, grid, tile picking |
| Input | `"Input"` | Input system |

---

## Log Levels

spdlog levels used in this project:

| Level | When to use |
|---|---|
| `TRACE` | Very verbose per-frame data (disabled in most builds) |
| `DEBUG` | Detailed diagnostic info (loading paths, cache hits) |
| `INFO` | Normal operational messages (file loaded, scene saved) |
| `WARN` | Something unexpected but recoverable (missing JSON field, fallback used) |
| `ERROR` | Something failed and recovery was attempted (file not found, parse failed) |
| `CRITICAL` | Unrecoverable failure (should almost never be used) |

In the MVP, `TRACE` is disabled at compile time to avoid performance cost.

---

## Logging Initialization

```
function Log::Init():
    // Create the console sink (stdout)
    consoleSink = make_shared<spdlog::sinks::stdout_color_sink_mt>()
    consoleSink.set_level(spdlog::level::debug)
    consoleSink.set_pattern("[%T] [%n] [%^%l%$] %v")
    
    // Create per-category loggers with the console sink
    for each category in { Engine, Editor, Renderer, Scene, Asset, Serialization, Iso, Input }:
        logger = make_shared<spdlog::logger>(category, consoleSink)
        logger.set_level(spdlog::level::debug)
        spdlog::register_logger(logger)
    
    // The ImGui sink is added later by the editor (see below)
```

The `Log::Init()` function lives in `engine/core/Log.cpp` and is called at the start of `main()`.

---

## Adding the ImGui Sink from the Editor

After the console panel is created, the editor registers a custom sink with each logger:

```
function EditorApp::InitLogging(consolePanel):
    imguiSink = make_shared<ImGuiConsoleSink>(consolePanel)
    imguiSink.set_level(spdlog::level::debug)
    
    for each categoryName in { "Engine", "Editor", "Renderer", ... }:
        logger = spdlog::get(categoryName)
        logger.sinks().push_back(imguiSink)
```

The `ImGuiConsoleSink` is a custom spdlog sink in `editor/panels/ConsolePanel.cpp`. The engine `Log` module does not know it exists.

---

## ImGuiConsoleSink

The sink stores incoming log messages in a thread-safe buffer that the `ConsolePanel` reads during its `OnImGuiRender()` call.

```
struct LogMessage {
    std::string text
    spdlog::level::level_enum level
    std::string loggerName
}

class ImGuiConsoleSink : public spdlog::sinks::base_sink<std::mutex>:
    
    function sink_it_(const spdlog::details::log_msg& msg):
        // Format the message
        text = format(msg)
        // Store in buffer
        m_buffer.push_back({ text, msg.level, msg.logger_name })
        // Auto-scroll flag
        m_scrollToBottom = true
    
    function flush_():
        pass  // nothing needed
    
    shared_ptr<ConsolePanel> m_panel
    vector<LogMessage> m_buffer
    bool m_scrollToBottom { false }
```

---

## Logging Macros

Convenience macros in `engine/core/Log.h` remove the need to call `spdlog::get()` at every log site:

```cpp
#define LOG_ENGINE_TRACE(...)    spdlog::get("Engine")->trace(__VA_ARGS__)
#define LOG_ENGINE_DEBUG(...)    spdlog::get("Engine")->debug(__VA_ARGS__)
#define LOG_ENGINE_INFO(...)     spdlog::get("Engine")->info(__VA_ARGS__)
#define LOG_ENGINE_WARN(...)     spdlog::get("Engine")->warn(__VA_ARGS__)
#define LOG_ENGINE_ERROR(...)    spdlog::get("Engine")->error(__VA_ARGS__)

#define LOG_RENDERER_INFO(...)   spdlog::get("Renderer")->info(__VA_ARGS__)
#define LOG_RENDERER_WARN(...)   spdlog::get("Renderer")->warn(__VA_ARGS__)
#define LOG_RENDERER_ERROR(...)  spdlog::get("Renderer")->error(__VA_ARGS__)

#define LOG_SCENE_INFO(...)      spdlog::get("Scene")->info(__VA_ARGS__)
#define LOG_SCENE_WARN(...)      spdlog::get("Scene")->warn(__VA_ARGS__)

#define LOG_ASSET_INFO(...)      spdlog::get("Asset")->info(__VA_ARGS__)
#define LOG_ASSET_WARN(...)      spdlog::get("Asset")->warn(__VA_ARGS__)
#define LOG_ASSET_ERROR(...)     spdlog::get("Asset")->error(__VA_ARGS__)

#define LOG_SERIAL_INFO(...)     spdlog::get("Serialization")->info(__VA_ARGS__)
#define LOG_SERIAL_WARN(...)     spdlog::get("Serialization")->warn(__VA_ARGS__)
#define LOG_SERIAL_ERROR(...)    spdlog::get("Serialization")->error(__VA_ARGS__)

#define LOG_ISO_DEBUG(...)       spdlog::get("Iso")->debug(__VA_ARGS__)
#define LOG_ISO_WARN(...)        spdlog::get("Iso")->warn(__VA_ARGS__)

#define LOG_INPUT_DEBUG(...)     spdlog::get("Input")->debug(__VA_ARGS__)

#define LOG_EDITOR_INFO(...)     spdlog::get("Editor")->info(__VA_ARGS__)
#define LOG_EDITOR_WARN(...)     spdlog::get("Editor")->warn(__VA_ARGS__)
#define LOG_EDITOR_ERROR(...)    spdlog::get("Editor")->error(__VA_ARGS__)
```

Usage:

```cpp
LOG_ASSET_INFO("Loaded texture: {} ({}x{})", path, width, height);
LOG_SERIAL_WARN("Missing field 'posX' in entity {}, using default 0.0", entityId);
LOG_RENDERER_ERROR("Failed to compile shader: {}", shaderPath);
```

---

## ConsolePanel Rendering

```
function ConsolePanel::OnImGuiRender():
    if not m_visible: return
    
    ImGui::Begin("Console", &m_visible)
    
    // Filter controls
    ImGui::SameLine()
    if ImGui::Button("Clear"):
        m_sink.m_buffer.clear()
    
    ImGui::SameLine()
    m_filter.Draw("Filter", 200)
    
    // Level toggles
    ImGui::SameLine()
    ToggleButton("DEBUG",  showDebug)
    ImGui::SameLine()
    ToggleButton("INFO",   showInfo)
    ImGui::SameLine()
    ToggleButton("WARN",   showWarn)
    ImGui::SameLine()
    ToggleButton("ERROR",  showError)
    
    ImGui::Separator()
    
    // Message list
    ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)
    
    for message in m_sink.m_buffer:
        if not PassesFilter(message): continue
        
        color = LevelToColor(message.level)
        ImGui::TextColored(color, "%s", message.text.c_str())
    
    if m_sink.m_scrollToBottom:
        ImGui::SetScrollHereY(1.0f)
        m_sink.m_scrollToBottom = false
    
    ImGui::EndChild()
    ImGui::End()
```

---

## Level Color Mapping

| Level | Color |
|---|---|
| DEBUG | Grey (0.6, 0.6, 0.6, 1.0) |
| INFO | White (1.0, 1.0, 1.0, 1.0) |
| WARN | Yellow (1.0, 1.0, 0.0, 1.0) |
| ERROR | Red (1.0, 0.3, 0.3, 1.0) |
| CRITICAL | Bright Red (1.0, 0.0, 0.0, 1.0) |

---

## File Sink (Later, Not MVP)

A file sink that writes to `logs/isoforge.log` can be added as an additional sink during `Log::Init()`. This is useful for debugging crashes after the fact. Not required in MVP.
