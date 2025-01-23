#pragma once

#include "WriterInterface.h"
#include <cstdint>
#include <memory>
#include <string>

namespace ps {

class Application {
private:
  std::string inputFileName;
  std::string outputFileName;

  std::string deviceName;
  uint64_t timeForListening = 60;

public:
  void setWriter(std::unique_ptr<IPCAPWriter> writer);

  void setDevice(std::string device);
  void setTimeForListening(uint64_t time);

  void setPcapFile(std::string fileName);
  void setOutputFile(std::string fileName);

  void start();
};

} // namespace PacketSniffer
