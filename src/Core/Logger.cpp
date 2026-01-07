#include "Logger.hpp"

#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <filesystem>
#include <memory>
#include <vector>

namespace Core
{
namespace
{
constexpr size_t kAsyncQueueSize = 8192;
constexpr size_t kMaxFileSize = 10ULL * 1024 * 1024;
constexpr size_t kMaxFiles = 3;
} // namespace

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init()
{
    if (!std::filesystem::exists("logs"))
    {
        std::filesystem::create_directory("logs");
    }

    spdlog::init_thread_pool(kAsyncQueueSize, 1);

    std::vector<spdlog::sink_ptr> sinks;

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern("%^[%T]: %v%$");
    sinks.push_back(consoleSink);

    auto fileSink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/AcousticFluids.log", kMaxFileSize, kMaxFiles);
    fileSink->set_pattern("[%Y-%m-%d %T] [%l] %v");
    sinks.push_back(fileSink);

    s_Logger = std::make_shared<spdlog::async_logger>(
        "APP", begin(sinks), end(sinks), spdlog::thread_pool(), spdlog::async_overflow_policy::overrun_oldest);

    spdlog::register_logger(s_Logger);

    s_Logger->set_level(spdlog::level::trace);
    s_Logger->flush_on(spdlog::level::err);
}

void Logger::Shutdown()
{
    if (s_Logger)
    {
        s_Logger->flush();
    }
    spdlog::shutdown();
    s_Logger.reset();
}

std::shared_ptr<spdlog::logger>& Logger::GetLogger()
{
    return s_Logger;
}
} // namespace Core
