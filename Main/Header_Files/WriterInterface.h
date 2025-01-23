#pragma once

#include "pcapplusplus/PcapLiveDevice.h"
#include "pcapplusplus/RawPacket.h"

namespace ps {

class IPCAPWriter {
public:
  virtual void Write(pcpp::RawPacket *rawPacket,
                     pcpp::PcapLiveDevice *dev = nullptr,
                     void *cookie = nullptr) = 0;
};

} // namespace ps
