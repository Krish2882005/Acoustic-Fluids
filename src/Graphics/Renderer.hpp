#pragma once

namespace Graphics
{
struct Color
{
    float R = 1.0F;
    float G = 1.0F;
    float B = 1.0F;
    float A = 1.0F;

    constexpr Color() = default;

    constexpr Color(float r, float g, float b, float a = 1.0F) noexcept : R(r), G(g), B(b), A(a) {}
};

class GPUContext;

class Renderer
{
public:
    explicit Renderer(GPUContext* context);
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = default;
    Renderer& operator=(Renderer&&) noexcept = default;

    void Draw(double alpha);

    void SetClearColor(const Color& color);
    void SetClearColor(float r, float g, float b, float a);

private:
    GPUContext* m_context;
    Color m_clearColor;
};
} // namespace Graphics
