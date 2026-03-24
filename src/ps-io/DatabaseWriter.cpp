#include "DatabaseWriter.h"

#include <Packet.h>

#include <ps-utils/Logger.h>

namespace ps
{
DatabaseWriter::DatabaseWriter()
{}
DatabaseWriter::DatabaseWriter(std::string connectionParameters)
{}

void DatabaseWriter::Write(pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* device, void* misc)
{
    auto packet = pcpp::Packet{rawPacket};
    if (!packet.isPacketOfType(pcpp::IPv4))
    {
        LOG_WARNING("Packet is not of IPv4 type");
    }
}

void DatabaseWriter::Flush()
{}

FullPacket::Ptr DatabaseWriter::parseFullPacket(const pcpp::Packet& packet)
{
    return {};
}
} // namespace ps
