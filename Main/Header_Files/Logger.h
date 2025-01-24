#pragma once

#include <mutex>
#include <string>

class Logger {
public:
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  static Logger &getInstance();

  void log(const std::string &message);
  void error(const std::string &message);

  void setVerbose(bool verbose);

private:
  Logger() : m_verbose(false) {};

  std::mutex m_lock;
  bool m_verbose = false;
};
