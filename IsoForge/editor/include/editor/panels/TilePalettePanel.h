#pragma once

#include "editor/EditorState.h"
#include "editor/panels/Panel.h"

namespace IsoForge
{
class TilePalettePanel : public Panel
{
public:
    explicit TilePalettePanel(EditorState& editorState);
    void OnImGuiRender() override;

private:
    EditorState& m_EditorState;
};
}
