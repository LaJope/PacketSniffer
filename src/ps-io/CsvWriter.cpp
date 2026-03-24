#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/Layer.h>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/TcpLayer.h>
#include <pcapplusplus/UdpLayer.h>

#include <ps-utils/Logger.h>

#include "CsvWriter.h"

namespace ps {

// csvPcapWriter public

CsvWriter::CsvWriter(std::string outputFileName)
    : m_outputFileName(outputFileName + ".csv") {}

void CsvWriter::Write(pcpp::RawPacket *rawPacket, pcpp::PcapLiveDevice *device,
                      void *cookie) {
  pcpp::Packet parsedPacket(rawPacket);
  if (!parsedPacket.isPacketOfType(pcpp::IPv4)) {
    Logger::getInstance().warning("Packet is not of IPv4 type");
    return;
  }

  std::optional<CollectData> ipAndPort = getDataKey(parsedPacket);
  if (!ipAndPort)
    return;

  int packetSize = parsedPacket.getRawPacket()->getFrameLength();

  {
    std::lock_guard<std::mutex> lock(m_dataAccessLock);

    auto it = m_data.find(ipAndPort.value());
    if (it != m_data.end()) {
      it->second.first++;
      it->second.second += packetSize;
      return;
    }
    m_data[ipAndPort.value()] = std::pair<int, int>{1, packetSize};
  }
}

void CsvWriter::Flush() {
  std::ofstream outputFile;
  outputFile.open(m_outputFileName);

  outputFile << "srcIP,dstIP,srcPort,dstPort,numPackets,totalSize";
  for (auto &elem : m_data) {
    const auto &srcIP = elem.first.srcIP.toInt();
    const auto &dstIP = elem.first.dstIP.toInt();
    const auto &srcPort = elem.first.srcPort;
    const auto &dstPort = elem.first.dstPort;
    outputFile << "\n"
               << srcIP << "," << dstIP << "," << srcPort << "," << dstPort
               << "," << elem.second.first << "," << elem.second.second;
  }
}

// csvPcapWriter private

std::optional<CollectData>
CsvWriter::getDataKey(const pcpp::Packet &packet) const {
  pcpp::IPv4Address srcIP, dstIP;
  uint16_t srcPort, dstPort;

  auto *ipLayer = packet.getLayerOfType<pcpp::IPv4Layer>();
  if (ipLayer == nullptr) {
    Logger::getInstance().warning("Couldn't find IPv4 layer...");
    return std::nullopt;
  }
  srcIP = ipLayer->getSrcIPv4Address();
  dstIP = ipLayer->getDstIPv4Address();

  auto *tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
  auto *udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
  if (tcpLayer == nullptr && udpLayer == nullptr) {
    Logger::getInstance().warning("Couldn't find TCP nor UDP layers...");
    return std::nullopt;
  }
  if (tcpLayer != nullptr) {
    srcPort = tcpLayer->getSrcPort();
    dstPort = tcpLayer->getDstPort();
  } else {
    srcPort = udpLayer->getSrcPort();
    dstPort = udpLayer->getDstPort();
  }

  CollectData result{srcIP, dstIP, srcPort, dstPort};

  return result;
}

}; // namespace ps
