#pragma once

#include "editor/EditorCamera.h"
#include "editor/EditorState.h"
#include "editor/panels/Panel.h"
#include "engine/iso/IsoMath.h"
#include "engine/iso/IsoGridRenderer.h"
#include "engine/iso/TilemapData.h"
#include "engine/renderer/Framebuffer.h"

#include <filesystem>
#include <string>

namespace IsoForge
{
class SceneViewportPanel : public Panel
{
public:
    explicit SceneViewportPanel(EditorState& editorState);
    [[nodiscard]] bool SaveTilemap();
    [[nodiscard]] bool SaveTilemap(const std::filesystem::path& path);
    [[nodiscard]] bool SaveTilemapAs(const std::filesystem::path& path);
    [[nodiscard]] bool LoadTilemap(const std::filesystem::path& path);
    [[nodiscard]] bool HasCurrentTilemapPath() const;
    [[nodiscard]] const std::filesystem::path& GetCurrentTilemapPath() const;
    [[nodiscard]] const std::string& GetLastTilemapIOStatus() const;
    void OnImGuiRender() override;

private:
    [[nodiscard]] static std::filesystem::path GetTilemapDirectory();

    EditorState& m_EditorState;
    EditorCamera m_EditorCamera;
    Framebuffer m_Framebuffer;
    TilemapData m_Tilemap {20, 20};
    HoverResult m_HoverResult;
    IsoGridRenderer m_IsoGridRenderer;
    std::filesystem::path m_CurrentTilemapPath;
    std::string m_LastTilemapIOStatus = "Tilemap IO: No save/load operation yet";
};
}
