#include "editor/EditorCamera.h"

#include <algorithm>

namespace IsoForge
{
void EditorCamera::Reset()
{
    m_OffsetX = 0.0f;
    m_OffsetY = 0.0f;
    m_Zoom = 1.0f;
}

void EditorCamera::Pan(float deltaX, float deltaY)
{
    m_OffsetX += deltaX;
    m_OffsetY += deltaY;
}

void EditorCamera::Zoom(float wheelDelta)
{
    if (wheelDelta == 0.0f)
    {
        return;
    }

    m_Zoom *= (wheelDelta > 0.0f) ? 1.1f : 0.9f;
    m_Zoom = std::clamp(m_Zoom, MinZoom, MaxZoom);
}

float EditorCamera::GetOffsetX() const
{
    return m_OffsetX;
}

float EditorCamera::GetOffsetY() const
{
    return m_OffsetY;
}

float EditorCamera::GetZoom() const
{
    return m_Zoom;
}
}
