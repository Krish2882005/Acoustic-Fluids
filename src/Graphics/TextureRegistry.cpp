#include "TextureRegistry.hpp"

#include <SDL3/SDL.h>

#include <cstdint>
#include <stdexcept>
#include <string>

#include "../Core/Logger.hpp"
#include "GPUContext.hpp"

namespace Graphics
{
const Texture2D& PingPongBuffer::GetRead() const
{
    return m_swapState ? m_textureB : m_textureA;
}

const Texture2D& PingPongBuffer::GetWrite() const
{
    return m_swapState ? m_textureA : m_textureB;
}

void PingPongBuffer::Swap()
{
    m_swapState = !m_swapState;
}

TextureRegistry::TextureRegistry(GPUContext* context) : m_context(context)
{
    if (!m_context)
    {
        throw std::runtime_error("TextureRegistry: Invalid GPUContext");
    }
}

TextureRegistry::~TextureRegistry()
{
    SDL_GPUDevice* device = m_context->GetDevice();

    for (auto& [name, buffer] : m_buffers)
    {
        if (buffer.m_textureA.Handle)
        {
            SDL_ReleaseGPUTexture(device, buffer.m_textureA.Handle);
        }
        if (buffer.m_textureB.Handle)
        {
            SDL_ReleaseGPUTexture(device, buffer.m_textureB.Handle);
        }
    }
    m_buffers.clear();
    LOG_INFO("TextureRegistry: Resources destroyed");
}

void TextureRegistry::CreatePingPong(const std::string& name, uint32_t width, uint32_t height, uint32_t preferredFormat)
{
    SDL_GPUDevice* device = m_context->GetDevice();

    const SDL_GPUTextureUsageFlags requiredUsage =
        SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE;

    auto chosenFormat = static_cast<SDL_GPUTextureFormat>(preferredFormat);

    if (!SDL_GPUTextureSupportsFormat(device, chosenFormat, SDL_GPU_TEXTURETYPE_2D, requiredUsage))
    {
        LOG_WARN("TextureRegistry: Format {} not supported for Compute/Sample. Trying fallback...",
                 static_cast<uint32_t>(chosenFormat));

        if (chosenFormat == SDL_GPU_TEXTUREFORMAT_R16_FLOAT)
        {
            chosenFormat = SDL_GPU_TEXTUREFORMAT_R32_FLOAT;
        }
        else if (chosenFormat == SDL_GPU_TEXTUREFORMAT_R16G16_FLOAT)
        {
            chosenFormat = SDL_GPU_TEXTUREFORMAT_R32G32_FLOAT;
        }
        else if (chosenFormat == SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT)
        {
            chosenFormat = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT;
        }

        if (!SDL_GPUTextureSupportsFormat(device, chosenFormat, SDL_GPU_TEXTURETYPE_2D, requiredUsage))
        {
            LOG_ERROR("TextureRegistry: Fatal - No suitable storage format found for '{}'", name);
            throw std::runtime_error("GPU Format Unsupported");
        }
    }

    PingPongBuffer buffer;
    const std::string nameA = name + "_A";
    const std::string nameB = name + "_B";

    buffer.m_textureA = CreateTextureInternal(width, height, chosenFormat, nameA.c_str());
    buffer.m_textureB = CreateTextureInternal(width, height, chosenFormat, nameB.c_str());

    m_buffers[name] = buffer;

    LOG_INFO("TextureRegistry: Created PingPong '{}' [{}x{}, Fmt:{}]",
             name,
             width,
             height,
             static_cast<uint32_t>(chosenFormat));
}

Texture2D
TextureRegistry::CreateTextureInternal(uint32_t width, uint32_t height, uint32_t format, const char* debugName)
{
    const SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_GPU_TEXTURE_CREATE_NAME_STRING, debugName);

    const SDL_GPUTextureCreateInfo createInfo = {.type = SDL_GPU_TEXTURETYPE_2D,
                                                 .format = static_cast<SDL_GPUTextureFormat>(format),
                                                 .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER |
                                                          SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE,
                                                 .width = width,
                                                 .height = height,
                                                 .layer_count_or_depth = 1,
                                                 .num_levels = 1,
                                                 .sample_count = SDL_GPU_SAMPLECOUNT_1,
                                                 .props = props};

    SDL_GPUTexture* handle = SDL_CreateGPUTexture(m_context->GetDevice(), &createInfo);

    SDL_DestroyProperties(props);

    if (!handle)
    {
        LOG_ERROR("TextureRegistry: Failed to create texture '{}': {}", debugName, SDL_GetError());
        throw std::runtime_error("Texture Creation Failed");
    }

    return Texture2D{.Handle = handle, .Width = width, .Height = height, .Format = format};
}

PingPongBuffer* TextureRegistry::GetBuffer(const std::string& name)
{
    if (m_buffers.contains(name))
    {
        return &m_buffers[name];
    }
    return nullptr;
}

void TextureRegistry::Swap(const std::string& name)
{
    if (m_buffers.contains(name))
    {
        m_buffers[name].Swap();
    }
}
} // namespace Graphics
