#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

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
  void setHelpToTrue();
  void setVerboseToTrue();
  void setOutfile(std::string fileName);
  void setInfile(std::string fileName);
  void setDevice(std::string deviceName);
  void setTime(std::string time);
};
