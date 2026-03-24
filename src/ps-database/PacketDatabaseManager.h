#pragma once

#include <memory>
#include <string>
#include <vector>

#include "DBConnector.h"
#include "PacketDataStructs.h"

namespace ps
{
class PacketDatabaseManager
{
  public:
    using Ptr = std::unique_ptr<PacketDatabaseManager>;

    explicit PacketDatabaseManager(DBConnector::Ptr dbConnector);
    ~PacketDatabaseManager();

    // Packet insertion methods
    bool insertPacket(const PacketData& packetData);
    bool insertPacketWithProtocolData(const FullPacket::Ptr packet);

    // Query methods
    std::vector<std::string> getProtocols();
    std::vector<std::string> getUniqueSources();
    std::vector<std::string> getUniqueDestinations();

    // Statistics methods
    std::string getPacketStatistics();
    std::string getDailyPacketCounts();
    std::string getTopTalkers();

    // Cleanup methods
    bool cleanupOldPackets(int daysOld = 30);

  private:
    DBConnector::Ptr m_dbConnector;

    // Helper methods for SQL generation
    template <typename T>
    std::string serializePacketData(const T& data);
};

template <typename T>
std::string PacketDatabaseManager::serializePacketData(const T& data)
{
    json dataAsJson = json::array({data});
    return dataAsJson.dump();
}

template <>
std::string PacketDatabaseManager::serializePacketData<PacketData>(const PacketData& packetData);

template <>
std::string PacketDatabaseManager::serializePacketData<IPPacketData>(const IPPacketData& ipData);

template <>
std::string PacketDatabaseManager::serializePacketData<TCPPacketData>(const TCPPacketData& tcpData);

template <>
std::string PacketDatabaseManager::serializePacketData<UDPPacketData>(const UDPPacketData& udpData);

template <>
std::string PacketDatabaseManager::serializePacketData<ICMPPacketData>(const ICMPPacketData& icmpData);

template <>
std::string PacketDatabaseManager::serializePacketData<ARPPacketData>(const ARPPacketData& arpData);
} // namespace ps
