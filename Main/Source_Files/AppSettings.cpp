#include <functional>
#include <stdexcept>
#include <optional>
#include <string>
#include <unordered_map>

#include "AppSettings.h"
#include "Logger.h"

// AppSettings public

AppSettings::AppSettings(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    std::string opt = argv[i];

    if (opt[0] == '-' && opt.length() > 2 && opt[1] != '-') {
      Logger::getInstance().warning(
          "Cannot combine multiple singular flags in one. Ignoring " + opt +
          " flag...");
      continue;
    }

    if (auto j = NoArgs.find(opt); j != NoArgs.end())
      j->second();

    else if (auto k = OneArgs.find(opt); k != OneArgs.end())
      if (++i < argc)
        k->second(argv[i]);
      else
        throw std::runtime_error{"Missing param after " + opt};

    else if (!m_infile)
      m_infile = argv[i];

    else
      Logger::getInstance().warning("Unrecognized command-line option " + opt +
                                  ". Ignoring...");
  }
}

// AppSettings private

void AppSettings::setHelpToTrue() { m_help = true; }
void AppSettings::setVerboseToTrue() { m_verbose = true; }
void AppSettings::setOutfile(std::string fileName) {
  m_outfile = fileName.substr(0, fileName.find("."));
}

void AppSettings::setInfile(std::string fileName) {
  if (!m_deviceName) {
    m_infile = fileName;
    return;
  }

  Logger::getInstance().warning("Cannot read from file and listen to a device at "
                              "the same time. Ignoring file flag...");
}

void AppSettings::setDevice(std::string deviceName) {
  if (!m_infile) {
    m_deviceName = deviceName;
    return;
  }

  Logger::getInstance().warning("Cannot read from file and listen to a device at "
                              "the same time. Ignoring device flag...");
}

void AppSettings::setTime(std::string time) { m_time = std::stoul(time); }
