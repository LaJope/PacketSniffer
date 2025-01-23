#pragma once

#include <memory>

#include "WriterInterface.h"

namespace ps {

void PcapFileSniffer(std::unique_ptr<IPCAPWriter> writer, std::string &filename);

} // namespace PacketSniffer
