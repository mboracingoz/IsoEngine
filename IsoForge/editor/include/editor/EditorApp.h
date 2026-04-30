#pragma once

#include "engine/core/Application.h"

namespace IsoForge
{
class EditorApp : public Application
{
public:
    EditorApp();
    ~EditorApp() override = default;

protected:
    void OnInit() override;
    void OnShutdown() override;
};
}
