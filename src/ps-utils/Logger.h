#pragma once

#include <fmt/core.h>
#include <mutex>
#include <stdexcept>

#define LOG_ERROR(fmt, ...) Logger::getInstance().error(fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) Logger::getInstance().warning(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Logger::getInstance().info(fmt, ##__VA_ARGS__)
#define LOG_THROW(fmt, ...) Logger::getInstance().throwError(fmt, ##__VA_ARGS__)

class Logger
{
public:
    inline static Logger& getInstance();
    inline void setVerbose(bool verbose);

    template<typename... Args>
    void info(fmt::format_string<Args...> format, Args&&... args);
    template<typename... Args>
    void warning(fmt::format_string<Args...> format, Args&&... args);
    template<typename... Args>
    void error(fmt::format_string<Args...> format, Args&&... args);
    template<typename... Args>
    void throwError(fmt::format_string<Args...> format, Args&&... args);

private:
    Logger() = default;
    ~Logger() = default;

    bool m_verbose = false;
    std::mutex m_lock;
};

inline Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

inline void Logger::setVerbose(bool verbose)
{
    m_verbose = verbose;
}

template<typename... Args>
void Logger::info(fmt::format_string<Args...> format, Args&&... args)
{
    if (m_verbose)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        fmt::print("[LOG] {}\n", fmt::format(format, std::forward<Args>(args)...));
    }
}

template<typename... Args>
void Logger::warning(fmt::format_string<Args...> format, Args&&... args)
{
    if (m_verbose)
    {
        std::lock_guard<std::mutex> lock(m_lock);
        fmt::print(stderr, "[WARNING] {}\n", fmt::format(format, std::forward<Args>(args)...));
    }
}

template<typename... Args>
void Logger::error(fmt::format_string<Args...> format, Args&&... args)
{
    std::lock_guard<std::mutex> lock(m_lock);
    fmt::print(stderr, "[ERROR] {}\n", fmt::format(format, std::forward<Args>(args)...));
}

template<typename... Args>
void Logger::throwError(fmt::format_string<Args...> format, Args&&... args)
{
    error(format, std::forward<Args>(args)...);
    throw std::runtime_error(fmt::format(format, std::forward<Args>(args)...));
}
