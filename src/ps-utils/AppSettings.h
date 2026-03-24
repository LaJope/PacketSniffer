#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "Logger.h"

class AppSettings
{
  public:
    bool m_help = false;
    bool m_verbose = false;

    std::optional<std::string> m_infile;
    std::string m_outfile = "pasn";

    std::optional<std::string> m_deviceName;
    uint32_t m_time = 60;

  public:
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(const AppSettings&) = delete;

    AppSettings(int argc, char* argv[]);

  private:
    using NoArgHandle = std::function<void()>;
    using OneArgHandle = std::function<void(std::string)>;

    const std::unordered_map<std::string, NoArgHandle> NoArgs{
        {"-h", [this]() { setHelpToTrue(); }},
        {"--help", [this]() { setHelpToTrue(); }},

        {"-v", [this]() { setVerboseToTrue(); }},
        {"--verbose", [this]() { setVerboseToTrue(); }},

    };

    const std::unordered_map<std::string, OneArgHandle> OneArgs{
        {"-i", [this](std::string v) { setInfile(v); }},  {"--infile", [this](std::string v) { setInfile(v); }},

        {"-o", [this](std::string v) { setOutfile(v); }}, {"--outfile", [this](std::string v) { setOutfile(v); }},

        {"-t", [this](std::string v) { setTime(v); }},    {"--time", [this](std::string v) { setTime(v); }},

        {"-d", [this](std::string v) { setDevice(v); }},  {"--device", [this](std::string v) { setDevice(v); }},
    };

  private:
    inline void setHelpToTrue();
    inline void setVerboseToTrue();
    inline void setTime(std::string time);
    inline void setOutfile(std::string fileName);
    inline void setInfile(std::string fileName);
    inline void setDevice(std::string deviceName);
};

inline void AppSettings::setHelpToTrue()
{
    m_help = true;
}

inline void AppSettings::setVerboseToTrue()
{
    m_verbose = true;
}

inline void AppSettings::setTime(std::string time)
{
    m_time = std::stoul(time);
}

inline void AppSettings::setOutfile(std::string fileName)
{
    m_outfile = fileName.substr(0, fileName.find("."));
}

inline void AppSettings::setInfile(std::string fileName)
{
    if (!m_deviceName)
    {
        LOG_ERROR("Cannot read from file and listen to a device at the same time. Ignoring file flag...");
        return;
    }

    m_infile = fileName;
}

inline void AppSettings::setDevice(std::string deviceName)
{
    if (m_infile)
    {
        LOG_ERROR("Cannot read from file and listen to a device at the same time. Ignoring device flag...");
        return;
    }

    m_deviceName = deviceName;
}
