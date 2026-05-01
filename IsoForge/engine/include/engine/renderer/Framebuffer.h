#pragma once

#include <cstdint>

namespace IsoForge
{
struct FramebufferSpec
{
    uint32_t width = 1280;
    uint32_t height = 720;
};

class Framebuffer
{
public:
    explicit Framebuffer(const FramebufferSpec& spec);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    void Bind() const;
    void Unbind() const;
    void Resize(uint32_t width, uint32_t height);
    void Clear(float r, float g, float b, float a) const;

    [[nodiscard]] uint32_t GetColorAttachmentID() const;
    [[nodiscard]] const FramebufferSpec& GetSpec() const;

private:
    void Invalidate();
    void Release();

private:
    FramebufferSpec m_Spec;
    uint32_t m_FramebufferID = 0;
    uint32_t m_ColorAttachmentID = 0;
    uint32_t m_DepthStencilAttachmentID = 0;
};
}
