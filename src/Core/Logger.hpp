#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace Core
{
class Logger
{
public:
    static void Init();
    static void Shutdown();

    static std::shared_ptr<spdlog::logger>& GetLogger();

    struct Scoped
    {
        Scoped() { Logger::Init(); }

        ~Scoped() { Logger::Shutdown(); }

        Scoped(const Scoped&) = delete;
        Scoped& operator=(const Scoped&) = delete;
        Scoped(Scoped&&) = delete;
        Scoped& operator=(Scoped&&) = delete;
    };

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};

} // namespace Core

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#if defined(NDEBUG)
#define LOG_TRACE(...) (void)0
#define LOG_DEBUG(...) (void)0
#else
#define LOG_TRACE(...) ::Core::Logger::GetLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...) ::Core::Logger::GetLogger()->debug(__VA_ARGS__)
#endif

#define LOG_INFO(...) ::Core::Logger::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Core::Logger::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Core::Logger::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Core::Logger::GetLogger()->critical(__VA_ARGS__)
// NOLINTEND(cppcoreguidelines-macro-usage)
