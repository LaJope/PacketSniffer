#include "PacketDatabaseManager.h"

#include <sstream>

#include <pqxx/pqxx>

#include <ps-utils/Logger.h>

#include "DBConnector.h"

namespace ps
{
PacketDatabaseManager::PacketDatabaseManager()
    : m_dbConnector(std::make_shared<DBConnector>())
{
    LOG_INFO("1");
    m_dbConnector->connect();
}

PacketDatabaseManager::PacketDatabaseManager(DBConnector::Ptr dbConnector)
    : m_dbConnector(dbConnector)
{
    LOG_INFO("2");
    m_dbConnector->connect();
}

PacketDatabaseManager::~PacketDatabaseManager()
{}

bool PacketDatabaseManager::insertPacket(const PacketData& packetData)
{
    LOG_INFO("3");
    try
    {
        std::string insertSql = "PERFORM insert_packet_with_protocol_data($1)";
        pqxx::params params = {json(packetData).dump()};

        m_dbConnector->executeQuery(insertSql, params);
        return true;
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to insert packet '{}'", exp.what());
        return false;
    }
}

bool PacketDatabaseManager::insertPacketWithProtocolData(const FullPacket::Ptr packet)
{
    LOG_INFO("4");
    if (!packet->packetData)
    {
        LOG_ERROR("No packet data provided. Could not write to database");
        return false;
    }

    try
    {
        json packetData{*packet->packetData};
        json protocolData;

        if (packet->ipPacketData)
            protocolData["IP"] = json{*packet->ipPacketData}[0];
        if (packet->tcpPacketData)
            protocolData["TCP"] = json{*packet->tcpPacketData}[0];
        if (packet->udpPacketData)
            protocolData["UDP"] = json{*packet->udpPacketData}[0];
        if (packet->icmpPacketData)
            protocolData["ICMP"] = json{*packet->icmpPacketData}[0];
        if (packet->arpPacketData)
            protocolData["ARP"] = json{*packet->arpPacketData}[0];

        LOG_INFO("Hello");
        if (protocolData.empty())
        {
            // LOG_INFO("1");
            m_dbConnector->executeQuery("CALL insert_packet_with_protocol_data($1)", pqxx::params{1});
        }
        else
        {
            if (!m_dbConnector)
            {
                LOG_INFO("HOHOHOHO");
                return false;
            }
            // LOG_INFO("2");
            m_dbConnector->executeQuery("CALL insert_packet_with_protocol_data($1, $2)", pqxx::params{1,2});
        }

        return true;
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to insert packet with protocol data: '{}'", exp.what());
        return false;
    }
}

std::vector<std::string> PacketDatabaseManager::getProtocols()
{
    std::vector<std::string> protocols;
    try
    {
        std::string selectSql = "SELECT DISTINCT protocol FROM packets ORDER BY protocol";
        pqxx::result result = m_dbConnector->executeQuery(selectSql);

        for (const auto& row : result)
            protocols.push_back(row[0].as<std::string>());
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to get protocols: '{}'", exp.what());
    }
    return protocols;
}

std::vector<std::string> PacketDatabaseManager::getUniqueSources()
{
    std::vector<std::string> sources;
    try
    {
        std::string selectSql = "SELECT DISTINCT source_ip FROM packets ORDER BY source_ip";
        pqxx::result result = m_dbConnector->executeQuery(selectSql);

        for (const auto& row : result)
            sources.push_back(row[0].as<std::string>());
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to get unique sources: '{}'", exp.what());
    }
    return sources;
}

std::vector<std::string> PacketDatabaseManager::getUniqueDestinations()
{
    std::vector<std::string> destinations;
    try
    {
        std::string selectSql = "SELECT DISTINCT destination_ip FROM packets ORDER BY destination_ip";
        pqxx::result result = m_dbConnector->executeQuery(selectSql);

        for (const auto& row : result)
            destinations.push_back(row[0].as<std::string>());
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to get unique destinations: '{}'", exp.what());
    }
    return destinations;
}

std::string PacketDatabaseManager::getPacketStatistics()
{
    try
    {
        std::string selectSql = "SELECT * FROM packet_statistics";
        pqxx::result result = m_dbConnector->executeQuery(selectSql);

        std::ostringstream oss;
        for (const auto& row : result)
        {
            oss << "Protocol: " << row[0].as<std::string>() << ", Count: " << row[1].as<int>()
                << ", First seen: " << row[2].as<std::string>() << ", Last seen: " << row[3].as<std::string>()
                << ", Unique sources: " << row[4].as<uint64_t>() << ", Unique destinations: " << row[5].as<uint64_t>()
                << ", Total bytes: " << row[6].as<uint64_t>() << "\n";
        }
        return oss.str();
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to get packet statistics: '{}'", exp.what());
        return "";
    }
}

std::string PacketDatabaseManager::getDailyPacketCounts()
{
    try
    {
        std::string selectSql = "SELECT * FROM daily_packet_counts";
        pqxx::result result = m_dbConnector->executeQuery(selectSql);

        std::ostringstream oss;
        for (const auto& row : result)
        {
            oss << "Date: " << row[0].as<std::string>() << ", Protocol: " << row[1].as<std::string>()
                << ", Count: " << row[2].as<uint64_t>() << ", Total bytes: " << row[3].as<uint64_t>() << "\n";
        }
        return oss.str();
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to get daily packet counts: '{}'", exp.what());
        return "";
    }
}

std::string PacketDatabaseManager::getTopTalkers()
{
    try
    {
        std::string selectSql = "SELECT * FROM top_talkers";
        pqxx::result result = m_dbConnector->executeQuery(selectSql);

        std::ostringstream oss;
        for (const auto& row : result)
        {
            oss << "Source IP: " << row[0].as<std::string>() << ", Destination IP: " << row[1].as<std::string>()
                << ", Protocol: " << row[2].as<std::string>() << ", Count: " << row[3].as<int>()
                << ", Total bytes: " << row[4].as<uint64_t>() << ", First seen: " << row[5].as<std::string>()
                << ", Last seen: " << row[6].as<std::string>() << "\n";
        }
        return oss.str();
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to get top talkers: '{}'", exp.what());
        return "";
    }
}

bool PacketDatabaseManager::cleanupOldPackets(int daysOld)
{
    try
    {
        std::string selectSql = "SELECT cleanup_old_packets($1)";
        pqxx::params params({std::to_string(daysOld)});
        m_dbConnector->executeQuery(selectSql, params);
        return true;
    }
    catch (const std::exception& exp)
    {
        LOG_ERROR("Failed to cleanup old packets: '{}'", exp.what());
        return false;
    }
}
} // namespace ps
