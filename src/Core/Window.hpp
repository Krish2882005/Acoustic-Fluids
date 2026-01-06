#pragma once

#include <memory>
#include <string>

struct SDL_Window;

namespace Core
{
class Window
{
public:
    Window(const std::string& title, uint32_t width, uint32_t height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

    void OnResize(uint32_t width, uint32_t height);

    [[nodiscard]] SDL_Window* GetNativeHandle() const;

    [[nodiscard]] uint32_t GetWidth() const;
    [[nodiscard]] uint32_t GetHeight() const;

private:
    struct WindowDestroyer
    {
        void operator()(SDL_Window* w) const;
    };

    using WindowPtr = std::unique_ptr<SDL_Window, WindowDestroyer>;

    WindowPtr m_window;
    uint32_t m_width{};
    uint32_t m_height{};
};
} // namespace Core
