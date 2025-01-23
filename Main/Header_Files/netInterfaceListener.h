#pragma once

#include <functional>

#include "WriterInterface.h"

#include "pcapplusplus/PcapLiveDevice.h"
#include "pcapplusplus/RawPacket.h"


namespace ps {

std::function<void(pcpp::RawPacket *, pcpp::PcapLiveDevice *, void *)>
netInterfaceListener(IPCAPWriter *writer);

} // namespace PacketSniffer
