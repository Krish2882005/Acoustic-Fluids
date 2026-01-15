#pragma once

#include <cstdint>

struct SDL_GPUDevice;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUShader;

namespace Graphics
{
class Shader;

struct BlendState
{
    bool EnableBlend = false;
    uint32_t SrcColorBlendFactor = 0;
    uint32_t DstColorBlendFactor = 0;
    uint32_t ColorBlendOp = 0;
    uint32_t SrcAlphaBlendFactor = 0;
    uint32_t DstAlphaBlendFactor = 0;
    uint32_t AlphaBlendOp = 0;
    uint8_t ColorWriteMask = 0;
};

class GraphicsPipelineBuilder
{
public:
    explicit GraphicsPipelineBuilder(SDL_GPUDevice* device);
    ~GraphicsPipelineBuilder() = default;

    GraphicsPipelineBuilder(const GraphicsPipelineBuilder&) = delete;
    GraphicsPipelineBuilder& operator=(const GraphicsPipelineBuilder&) = delete;
    GraphicsPipelineBuilder(GraphicsPipelineBuilder&&) noexcept = default;
    GraphicsPipelineBuilder& operator=(GraphicsPipelineBuilder&&) noexcept = default;

    [[nodiscard]] SDL_GPUGraphicsPipeline* Build();

    GraphicsPipelineBuilder& SetVertexShader(const Shader* shader);
    GraphicsPipelineBuilder& SetFragmentShader(const Shader* shader);
    GraphicsPipelineBuilder& SetOutputPixelFormat(uint32_t format);
    GraphicsPipelineBuilder& SetBlendState(const BlendState& state);

private:
    SDL_GPUDevice* m_device = nullptr;
    SDL_GPUShader* m_vertexShader = nullptr;
    SDL_GPUShader* m_fragmentShader = nullptr;
    uint32_t m_colorFormat = 0;
    BlendState m_blendState{};
};
} // namespace Graphics
