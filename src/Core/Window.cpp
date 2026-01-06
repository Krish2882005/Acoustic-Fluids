#include "Window.hpp"

#include <SDL3/SDL.h>

#include <cstdint>
#include <print>
#include <stdexcept>
#include <string>

namespace Core
{
void Window::WindowDestroyer::operator()(SDL_Window* w) const
{
    if (w)
    {
        SDL_DestroyWindow(w);
    }
}

Window::Window(const std::string& title, uint32_t width, uint32_t height) : m_width(width), m_height(height)
{
    m_window.reset(SDL_CreateWindow(title.c_str(),
                                    static_cast<int>(width),
                                    static_cast<int>(height),
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY));

    if (!m_window)
    {
        throw std::runtime_error(SDL_GetError());
    }

    std::println("Window: Created '{}' ({}x{})", title, width, height);
}

Window::~Window() = default;

Window::Window(Window&&) noexcept = default;
Window& Window::operator=(Window&&) noexcept = default;

void Window::OnResize(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    std::println("Window: Resized to {}x{}", width, height);
}

SDL_Window* Window::GetNativeHandle() const
{
    return m_window.get();
}

uint32_t Window::GetWidth() const
{
    return m_width;
}

uint32_t Window::GetHeight() const
{
    return m_height;
}
} // namespace Core
