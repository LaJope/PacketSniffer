#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <utility>

#include "IWriter.h"

namespace ps
{

struct CollectData
{
    pcpp::IPv4Address srcIP;
    pcpp::IPv4Address dstIP;
    uint16_t srcPort;
    uint16_t dstPort;

    CollectData(pcpp::IPv4Address sIP, pcpp::IPv4Address dIP, uint16_t sPort, uint16_t dPort)
        : srcIP{sIP}
        , dstIP{dIP}
        , srcPort{sPort}
        , dstPort{dPort}
    {}

    bool operator==(const CollectData& other) const
    {
        return srcIP == other.srcIP && dstIP == other.dstIP && srcPort == other.srcPort && dstPort == other.dstPort;
    }
};

struct CollectDataHash
{
    std::size_t operator()(const CollectData& data) const
    {
        uint64_t first = (std::hash<uint32_t>()(data.srcIP.toInt()) ^ std::hash<uint16_t>()(data.srcPort)) << 32;
        uint64_t second = (std::hash<uint32_t>()(data.dstIP.toInt()) ^ std::hash<uint16_t>()(data.dstPort));
        return first + second;
    }
};

class CsvWriter : public IPacketWriter
{
  public:
    CsvWriter(std::string);

    void Write(pcpp::RawPacket*, pcpp::PcapLiveDevice* = nullptr, void* = nullptr) override;

    void Flush() override;

  private:
    std::unordered_map<CollectData, std::pair<int, int>, CollectDataHash> m_data;

    std::string m_outputFileName;

    std::mutex m_dataAccessLock;

  private:
    std::optional<CollectData> getDataKey(const pcpp::Packet&) const;
};

} // namespace ps
