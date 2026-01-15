#include "PipelineBuilder.hpp"

#include <SDL3/SDL_gpu.h>

#include <cstdint>
#include <stdexcept>

#include "../Core/Logger.hpp"
#include "Shader.hpp"

namespace Graphics
{
namespace
{
constexpr Uint8 kColorWriteMaskAll = 0xF;
constexpr SDL_GPUTextureFormat kDefaultFormat = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
} // namespace

GraphicsPipelineBuilder::GraphicsPipelineBuilder(SDL_GPUDevice* device)
    : m_device(device), m_colorFormat(static_cast<uint32_t>(kDefaultFormat))
{
    m_blendState = {.EnableBlend = false,
                    .SrcColorBlendFactor = SDL_GPU_BLENDFACTOR_ONE,
                    .DstColorBlendFactor = SDL_GPU_BLENDFACTOR_ZERO,
                    .ColorBlendOp = SDL_GPU_BLENDOP_ADD,
                    .SrcAlphaBlendFactor = SDL_GPU_BLENDFACTOR_ONE,
                    .DstAlphaBlendFactor = SDL_GPU_BLENDFACTOR_ZERO,
                    .AlphaBlendOp = SDL_GPU_BLENDOP_ADD,
                    .ColorWriteMask = kColorWriteMaskAll};
}

SDL_GPUGraphicsPipeline* GraphicsPipelineBuilder::Build()
{
    if (!m_vertexShader || !m_fragmentShader)
    {
        LOG_ERROR("PipelineBuilder: Missing shaders");
        return nullptr;
    }

    const SDL_GPUColorTargetDescription colorTargetDesc{
        .format = static_cast<SDL_GPUTextureFormat>(m_colorFormat),
        .blend_state = {.src_color_blendfactor = static_cast<SDL_GPUBlendFactor>(m_blendState.SrcColorBlendFactor),
                        .dst_color_blendfactor = static_cast<SDL_GPUBlendFactor>(m_blendState.DstColorBlendFactor),
                        .color_blend_op = static_cast<SDL_GPUBlendOp>(m_blendState.ColorBlendOp),
                        .src_alpha_blendfactor = static_cast<SDL_GPUBlendFactor>(m_blendState.SrcAlphaBlendFactor),
                        .dst_alpha_blendfactor = static_cast<SDL_GPUBlendFactor>(m_blendState.DstAlphaBlendFactor),
                        .alpha_blend_op = static_cast<SDL_GPUBlendOp>(m_blendState.AlphaBlendOp),
                        .color_write_mask = m_blendState.ColorWriteMask,
                        .enable_blend = m_blendState.EnableBlend}};

    const SDL_GPUGraphicsPipelineCreateInfo info{.vertex_shader = m_vertexShader,
                                                 .fragment_shader = m_fragmentShader,
                                                 .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
                                                 .rasterizer_state =
                                                     {
                                                         .fill_mode = SDL_GPU_FILLMODE_FILL,
                                                         .cull_mode = SDL_GPU_CULLMODE_NONE,
                                                         .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
                                                     },
                                                 .target_info = {
                                                     .color_target_descriptions = &colorTargetDesc,
                                                     .num_color_targets = 1,
                                                 }};

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &info);
    if (!pipeline)
    {
        LOG_ERROR("PipelineBuilder: Failed to create graphics pipeline: {}", SDL_GetError());
        throw std::runtime_error("Pipeline Creation Failed");
    }
    return pipeline;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetVertexShader(const Shader* shader)
{
    if (shader && shader->GetStage() == ShaderStage::Vertex)
    {
        m_vertexShader = shader->GetGraphicsHandle();
    }
    else
    {
        LOG_ERROR("PipelineBuilder: Invalid Vertex Shader");
    }
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetFragmentShader(const Shader* shader)
{
    if (shader && shader->GetStage() == ShaderStage::Fragment)
    {
        m_fragmentShader = shader->GetGraphicsHandle();
    }
    else
    {
        LOG_ERROR("PipelineBuilder: Invalid Fragment Shader");
    }
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetOutputPixelFormat(uint32_t format)
{
    m_colorFormat = format;
    return *this;
}

GraphicsPipelineBuilder& GraphicsPipelineBuilder::SetBlendState(const BlendState& state)
{
    m_blendState = state;
    return *this;
}
} // namespace Graphics
