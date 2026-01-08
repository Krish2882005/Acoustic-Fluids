#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

struct SDL_GPUTexture;

namespace Graphics
{
class GPUContext;

struct Texture2D
{
    SDL_GPUTexture* Handle = nullptr;
    uint32_t Width = 0;
    uint32_t Height = 0;
    uint32_t Format = 0;
};

class PingPongBuffer
{
public:
    [[nodiscard]] const Texture2D& GetRead() const;
    [[nodiscard]] const Texture2D& GetWrite() const;

    void Swap();

private:
    friend class TextureRegistry;

    Texture2D m_textureA;
    Texture2D m_textureB;
    bool m_swapState = false;
};

class TextureRegistry
{
public:
    explicit TextureRegistry(GPUContext* context);
    ~TextureRegistry();

    TextureRegistry(const TextureRegistry&) = delete;
    TextureRegistry& operator=(const TextureRegistry&) = delete;
    TextureRegistry(TextureRegistry&&) noexcept = default;
    TextureRegistry& operator=(TextureRegistry&&) noexcept = default;

    void CreatePingPong(const std::string& name, uint32_t width, uint32_t height, uint32_t preferredFormat);

    [[nodiscard]] PingPongBuffer* GetBuffer(const std::string& name);

    void Swap(const std::string& name);

private:
    Texture2D CreateTextureInternal(uint32_t width, uint32_t height, uint32_t format, const char* debugName);

    GPUContext* m_context;
    std::unordered_map<std::string, PingPongBuffer> m_buffers;
};
} // namespace Graphics
