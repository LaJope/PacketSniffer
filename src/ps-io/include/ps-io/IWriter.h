#pragma once

#include <memory>

#include <pcapplusplus/PcapLiveDevice.h>
#include <pcapplusplus/RawPacket.h>

namespace ps {

class IPacketWriter {
public:
  using Ptr = std::shared_ptr<IPacketWriter>;

  virtual void Write(pcpp::RawPacket *rawPacket,
                     pcpp::PcapLiveDevice *dev = nullptr,
                     void *cookie = nullptr) = 0;

  virtual void Flush() = 0;
};

} // namespace ps
