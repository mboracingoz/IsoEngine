#pragma once

#include "engine/renderer/Texture2D.h"

#include <filesystem>
#include <memory>
#include <unordered_map>

namespace IsoForge
{
class AssetManager
{
public:
    explicit AssetManager(std::filesystem::path projectRoot);

    std::shared_ptr<Texture2D> LoadTexture(const std::filesystem::path& relativePath);
    std::shared_ptr<Texture2D> GetMissingTexture();

    [[nodiscard]] const std::filesystem::path& GetProjectRoot() const;

private:
    [[nodiscard]] std::filesystem::path ResolvePath(const std::filesystem::path& relativePath) const;
    std::shared_ptr<Texture2D> CreateMissingTexture();

private:
    std::filesystem::path m_ProjectRoot;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_TextureCache;
    std::shared_ptr<Texture2D> m_MissingTexture;
};
}
