#include "engine/renderer/Framebuffer.h"

#include <glad/glad.h>

#include <algorithm>
#include <iostream>

namespace IsoForge
{
Framebuffer::Framebuffer(const FramebufferSpec& spec)
    : m_Spec(spec)
{
    m_Spec.width = std::max(m_Spec.width, 1u);
    m_Spec.height = std::max(m_Spec.height, 1u);
    Invalidate();
}

Framebuffer::~Framebuffer()
{
    Release();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : m_Spec(other.m_Spec)
    , m_FramebufferID(other.m_FramebufferID)
    , m_ColorAttachmentID(other.m_ColorAttachmentID)
    , m_DepthStencilAttachmentID(other.m_DepthStencilAttachmentID)
{
    other.m_Spec = {};
    other.m_FramebufferID = 0;
    other.m_ColorAttachmentID = 0;
    other.m_DepthStencilAttachmentID = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Release();

    m_Spec = other.m_Spec;
    m_FramebufferID = other.m_FramebufferID;
    m_ColorAttachmentID = other.m_ColorAttachmentID;
    m_DepthStencilAttachmentID = other.m_DepthStencilAttachmentID;

    other.m_Spec = {};
    other.m_FramebufferID = 0;
    other.m_ColorAttachmentID = 0;
    other.m_DepthStencilAttachmentID = 0;

    return *this;
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
    glViewport(0, 0, static_cast<GLsizei>(m_Spec.width), static_cast<GLsizei>(m_Spec.height));
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0)
    {
        return;
    }

    width = std::max(width, 1u);
    height = std::max(height, 1u);

    if (m_Spec.width == width && m_Spec.height == height)
    {
        return;
    }

    m_Spec.width = width;
    m_Spec.height = height;
    Invalidate();
}

void Framebuffer::Clear(float r, float g, float b, float a) const
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

uint32_t Framebuffer::GetColorAttachmentID() const
{
    return m_ColorAttachmentID;
}

const FramebufferSpec& Framebuffer::GetSpec() const
{
    return m_Spec;
}

void Framebuffer::Invalidate()
{
    Release();

    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    glGenTextures(1, &m_ColorAttachmentID);
    glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        static_cast<GLsizei>(m_Spec.width),
        static_cast<GLsizei>(m_Spec.height),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        m_ColorAttachmentID,
        0
    );

    glGenRenderbuffers(1, &m_DepthStencilAttachmentID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilAttachmentID);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        static_cast<GLsizei>(m_Spec.width),
        static_cast<GLsizei>(m_Spec.height)
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        m_DepthStencilAttachmentID
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer is incomplete." << '\n';
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Release()
{
    if (m_DepthStencilAttachmentID != 0)
    {
        glDeleteRenderbuffers(1, &m_DepthStencilAttachmentID);
        m_DepthStencilAttachmentID = 0;
    }

    if (m_ColorAttachmentID != 0)
    {
        glDeleteTextures(1, &m_ColorAttachmentID);
        m_ColorAttachmentID = 0;
    }

    if (m_FramebufferID != 0)
    {
        glDeleteFramebuffers(1, &m_FramebufferID);
        m_FramebufferID = 0;
    }
}
}
