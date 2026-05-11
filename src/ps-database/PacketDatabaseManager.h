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

    explicit PacketDatabaseManager();
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
};
} // namespace ps
