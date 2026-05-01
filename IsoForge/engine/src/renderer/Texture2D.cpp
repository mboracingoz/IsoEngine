#include "engine/renderer/Texture2D.h"

#include <glad/glad.h>

namespace IsoForge
{
Texture2D::Texture2D(int width, int height, const unsigned char* pixels)
    : m_Width(width)
    , m_Height(height)
{
    if (width <= 0 || height <= 0 || pixels == nullptr)
    {
        m_Width = 0;
        m_Height = 0;
        return;
    }

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::~Texture2D()
{
    Release();
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : m_RendererID(other.m_RendererID)
    , m_Width(other.m_Width)
    , m_Height(other.m_Height)
{
    other.m_RendererID = 0;
    other.m_Width = 0;
    other.m_Height = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Release();
    m_RendererID = other.m_RendererID;
    m_Width = other.m_Width;
    m_Height = other.m_Height;
    other.m_RendererID = 0;
    other.m_Width = 0;
    other.m_Height = 0;
    return *this;
}

bool Texture2D::IsValid() const
{
    return m_RendererID != 0;
}

uint32_t Texture2D::GetRendererID() const
{
    return m_RendererID;
}

int Texture2D::GetWidth() const
{
    return m_Width;
}

int Texture2D::GetHeight() const
{
    return m_Height;
}

void Texture2D::Release()
{
    if (m_RendererID != 0)
    {
        glDeleteTextures(1, &m_RendererID);
        m_RendererID = 0;
    }

    m_Width = 0;
    m_Height = 0;
}
}
