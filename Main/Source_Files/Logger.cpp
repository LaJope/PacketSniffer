#include "Logger.h"
#include <iostream>

Logger &Logger::getInstance() {
  static Logger instance;
  return instance;
}

// Log a message if verbose is enabled
void Logger::log(const std::string &message) {
  if (m_verbose) {
    std::lock_guard<std::mutex> lock(m_lock); // Ensure thread safety
    std::cout << "[LOG] " << message << std::endl;
  }
}

// Log an error message (always printed)
void Logger::error(const std::string &message) {
  std::lock_guard<std::mutex> lock(m_lock); // Ensure thread safety
  std::cerr << "[ERROR] " << message << std::endl;
}

// Enable or disable verbose logging
void Logger::setVerbose(bool verbose) { m_verbose = verbose; }
