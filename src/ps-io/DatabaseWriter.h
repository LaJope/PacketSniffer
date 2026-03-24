#pragma once

#include <PcapLiveDevice.h>
#include <RawPacket.h>

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

    FullPacket::Ptr parseFullPacket(const pcpp::Packet& packet);
};
} // namespace ps
