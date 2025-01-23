#pragma once

#include <iostream>

#include "WriterInterface.h"

namespace ps {

class csvPcapWriter : public IPCAPWriter {

public:
  void Write(pcpp::RawPacket *rawPacket, pcpp::PcapLiveDevice *dev = nullptr,
             void *cookie = nullptr) override {
    std::cout << "Hello" << std::endl;
  }
};

} // namespace ps
