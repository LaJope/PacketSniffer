#pragma once

#include <ps-database/PacketDataStructs.h>
#include <ps-database/PacketDatabaseManager.h>
#include <ps-io/IWriter.h>

namespace ps
{
class DatabaseWriter : public IPacketWriter
{
  public:
    DatabaseWriter();
    DatabaseWriter(std::string connectionParameters);

    virtual void Write(pcpp::RawPacket*, pcpp::PcapLiveDevice* = nullptr, void* = nullptr) override;

    virtual void Flush() override;

  private:
    PacketDatabaseManager::Ptr m_packetDatabaseManager;

    FullPacket::Ptr parseFullPacket(const pcpp::Packet& packet, const std::string& devName);
    PacketData::Ptr parsePacketData(const pcpp::Packet& packet, const std::string& devName);
    IPPacketData::Ptr parsePacketIPData(const pcpp::Packet& packet);
    TCPPacketData::Ptr parsePacketTCPData(const pcpp::Packet& packet);
    UDPPacketData::Ptr parsePacketUDPData(const pcpp::Packet& packet);
    ICMPPacketData::Ptr parsePacketICMPData(const pcpp::Packet& packet);
    ARPPacketData::Ptr parsePacketARPData(const pcpp::Packet& packet);

};
} // namespace ps
