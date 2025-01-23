#include <functional>

#include "WriterInterface.h"

#include "pcapplusplus/PcapLiveDevice.h"
#include "pcapplusplus/RawPacket.h"

namespace ps {

std::function<void(pcpp::RawPacket *, pcpp::PcapLiveDevice *, void *)>
netInterfaceListener(IPCAPWriter *writer) {
  return [writer](pcpp::RawPacket *rawPacket, pcpp::PcapLiveDevice *dev,
                  void *cookie) { writer->Write(rawPacket, dev, cookie); };
}

} // namespace PacketSniffer
