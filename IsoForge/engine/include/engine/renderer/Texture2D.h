#pragma once

#include <cstdint>

namespace IsoForge
{
class Texture2D
{
public:
    Texture2D() = default;
    Texture2D(int width, int height, const unsigned char* pixels);
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] uint32_t GetRendererID() const;
    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;

private:
    void Release();

private:
    uint32_t m_RendererID = 0;
    int m_Width = 0;
    int m_Height = 0;
};
}
