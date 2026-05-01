#include "engine/assets/AssetManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <utility>

namespace IsoForge
{
AssetManager::AssetManager(std::filesystem::path projectRoot)
    : m_ProjectRoot(std::move(projectRoot))
{
    std::filesystem::create_directories(m_ProjectRoot / "assets");
    std::filesystem::create_directories(m_ProjectRoot / "assets" / "textures");
}

std::shared_ptr<Texture2D> AssetManager::LoadTexture(const std::filesystem::path& relativePath)
{
    if (relativePath.empty())
    {
        std::cerr << "Cannot load texture with an empty relative path." << '\n';
        return GetMissingTexture();
    }

    const std::string cacheKey = relativePath.lexically_normal().generic_string();
    const auto cacheIt = m_TextureCache.find(cacheKey);
    if (cacheIt != m_TextureCache.end())
    {
        return cacheIt->second;
    }

    const std::filesystem::path fullPath = ResolvePath(relativePath);
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* pixelData = stbi_load(fullPath.string().c_str(), &width, &height, &channels, 4);
    if (pixelData == nullptr)
    {
        std::cerr << "Failed to load texture: " << fullPath << '\n';
        return GetMissingTexture();
    }

    std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(width, height, pixelData);
    stbi_image_free(pixelData);

    if (!texture->IsValid())
    {
        std::cerr << "Failed to create GPU texture for: " << fullPath << '\n';
        return GetMissingTexture();
    }

    m_TextureCache[cacheKey] = texture;
    return texture;
}

std::shared_ptr<Texture2D> AssetManager::GetMissingTexture()
{
    if (!m_MissingTexture)
    {
        m_MissingTexture = CreateMissingTexture();
    }

    return m_MissingTexture;
}

const std::filesystem::path& AssetManager::GetProjectRoot() const
{
    return m_ProjectRoot;
}

std::filesystem::path AssetManager::ResolvePath(const std::filesystem::path& relativePath) const
{
    return (m_ProjectRoot / relativePath).lexically_normal();
}

std::shared_ptr<Texture2D> AssetManager::CreateMissingTexture()
{
    constexpr std::array<unsigned char, 4 * 4 * 4> pixels = {
        255, 0, 255, 255,   0, 0, 0, 255,   255, 0, 255, 255,   0, 0, 0, 255,
        0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,   255, 0, 255, 255,
        255, 0, 255, 255,   0, 0, 0, 255,   255, 0, 255, 255,   0, 0, 0, 255,
        0, 0, 0, 255,       255, 0, 255, 255,   0, 0, 0, 255,   255, 0, 255, 255
    };

    return std::make_shared<Texture2D>(4, 4, pixels.data());
}
}
