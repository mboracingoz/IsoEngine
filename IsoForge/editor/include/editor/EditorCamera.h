#pragma once

namespace IsoForge
{
class EditorCamera
{
public:
    EditorCamera() = default;

    void Reset();

    void Pan(float deltaX, float deltaY);
    void Zoom(float wheelDelta);

    [[nodiscard]] float GetOffsetX() const;
    [[nodiscard]] float GetOffsetY() const;
    [[nodiscard]] float GetZoom() const;

private:
    float m_OffsetX = 0.0f;
    float m_OffsetY = 0.0f;
    float m_Zoom = 1.0f;

    static constexpr float MinZoom = 0.5f;
    static constexpr float MaxZoom = 4.0f;
};
}
