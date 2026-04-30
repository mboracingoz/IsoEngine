#pragma once

#include "editor/panels/Panel.h"

namespace IsoForge
{
class AssetBrowserPanel : public Panel
{
public:
    AssetBrowserPanel();
    void OnImGuiRender() override;
};
}
