#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "Logger.h"

class AppSettings {
public:
  bool m_help = false;
  bool m_verbose = false;

  std::optional<std::string> m_infile;
  std::string m_outfile = "pasn";

  std::optional<std::string> m_deviceName;
  uint32_t m_time = 60;

public:
  AppSettings(int argc, char *argv[]);

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
      {"-i", [this](std::string v) { setInfile(v); }},
      {"--infile", [this](std::string v) { setInfile(v); }},

      {"-o", [this](std::string v) { setOutfile(v); }},
      {"--outfile", [this](std::string v) { setOutfile(v); }},

      {"-t", [this](std::string v) { setTime(v); }},
      {"--time", [this](std::string v) { setTime(v); }},

      {"-d", [this](std::string v) { setDevice(v); }},
      {"--device", [this](std::string v) { setDevice(v); }},
  };

private:
  inline void setHelpToTrue() { m_help = true; }
  inline void setVerboseToTrue() { m_verbose = true; }
  inline void setTime(std::string time) { m_time = std::stoul(time); }

  inline void setOutfile(std::string fileName) {
    m_outfile = fileName.substr(0, fileName.find("."));
  }
  inline void setInfile(std::string fileName) {
    if (!m_deviceName) {
      m_infile = fileName;
      return;
    }

    Logger::getInstance().error(
        "Cannot read from file and listen to a device at "
        "the same time. Ignoring file flag...");
  }

  inline void setDevice(std::string deviceName) {
    if (!m_infile) {
      m_deviceName = deviceName;
      return;
    }

    Logger::getInstance().error(
        "Cannot read from file and listen to a device at "
        "the same time. Ignoring device flag...");
  }
};
