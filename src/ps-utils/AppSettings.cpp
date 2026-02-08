#include "AppSettings.h"

#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Logger.h"

AppSettings::AppSettings(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    std::string opt = argv[i];

    if (opt[0] == '-' && opt.length() > 2 && opt[1] != '-') {
      Logger::getInstance().error(
          "Cannot combine multiple singular flags in one. Ignoring " + opt +
          " flag...");
      continue;
    }

    if (auto j = NoArgs.find(opt); j != NoArgs.end()) {
      j->second();
      continue;
    }

    if (auto k = OneArgs.find(opt); k != OneArgs.end()) {
      if (++i < argc)
        k->second(argv[i]);
      else
        throw std::runtime_error{"Missing param after " + opt};
      continue;
    }

    if (!m_infile) {
      m_infile = argv[i];
      continue;
    }

    Logger::getInstance().error("Unrecognized command-line option " + opt +
                                ". Ignoring...");
  }
}
