#include "Logger.h"
#include <iostream>

Logger &Logger::getInstance() {
  static Logger instance;
  return instance;
}

void Logger::log(const std::string &message) {
  if (m_verbose) {
    std::lock_guard<std::mutex> lock(m_lock);
    std::cout << "[LOG] " << message << std::endl;
  }
}

void Logger::warning(const std::string &message) {
  if (m_verbose) {
    std::lock_guard<std::mutex> lock(m_lock);
    std::cerr << "[WARNING] " << message << std::endl;
  }
}

void Logger::error(const std::string &message) {
  std::lock_guard<std::mutex> lock(m_lock);
  std::cerr << "[ERROR] " << message << std::endl;
}

void Logger::setVerbose(bool verbose) { m_verbose = verbose; }
