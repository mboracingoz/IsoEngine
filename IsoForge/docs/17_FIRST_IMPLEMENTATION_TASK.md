# 17 — First Implementation Task

## Task

Implement **Phase 0 only**: repository and documentation setup.

---

## Scope

Create the base repository skeleton:

- Root CMake files
- Formatting and ignore files
- `docs/` documentation layout
- `third_party/` placeholder structure
- `engine/` interface-only setup
- `editor/` stub executable with a minimal `main.cpp`

---

## Do Not Implement Yet

- SDL3 integration
- OpenGL calls
- glad initialization
- Dear ImGui
- Renderer code
- Editor panels
- Scene / ECS code
- FetchContent declarations

---

## Acceptance

1. Configure with the debug preset.
2. Build the `isoforge_editor` target.
3. Run the editor stub and confirm exit code `0`.
4. Verify `compile_commands.json` exists in the debug build directory.
5. Verify `engine/src` contains no real source files.
