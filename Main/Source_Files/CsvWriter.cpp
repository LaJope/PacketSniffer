#include <iostream>
#include <optional>
#include <string>
#include <utility>

#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/TcpLayer.h>
#include <pcapplusplus/UdpLayer.h>

#include "CsvWriter.h"

namespace ps {

// csvPcapWriter public

CsvWriter::CsvWriter(std::string outputFileName)
    : m_outputFileName(outputFileName + ".csv") {}

void CsvWriter::Write(pcpp::RawPacket *rawPacket, pcpp::PcapLiveDevice *device,
                      void *cookie) {

  pcpp::Packet parsedPacket(rawPacket);
  if (!parsedPacket.isPacketOfType(pcpp::IPv4)) {
    std::cerr << "Packet is not of IPv4 type" << std::endl;
    return;
  }

  std::optional<std::string> ipAndPort = getDataKey(parsedPacket);
  if (!ipAndPort)
    return;

  int packetSize = parsedPacket.getRawPacket()->getFrameLength();

  auto it = m_data.find(ipAndPort.value());
  if (it != m_data.end()) {
    it->second.first++;
    it->second.second += packetSize;
    return;
  }
  m_data.insert_or_assign(ipAndPort.value(),
                          std::pair<int, int>{1, packetSize});
}

void CsvWriter::Flush() {
  std::cout << m_outputFileName << "\n" << m_data.size() << std::endl;
}

// csvPcapWriter private

std::optional<std::string>
CsvWriter::getDataKey(const pcpp::Packet &packet) const {
  std::string srcIPAddres, distIPAddres;
  std::string srcPort, distPort;

  auto *ipLayer = packet.getLayerOfType<pcpp::IPv4Layer>();
  if (ipLayer == nullptr) {
    std::cerr << "Couldn't find IPv4 layer..." << std::endl;
    return std::nullopt;
  }

  srcIPAddres = ipLayer->getSrcIPv4Address().toString();
  distIPAddres = ipLayer->getDstIPv4Address().toString();

  auto *tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
  auto *udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
  if (tcpLayer == nullptr && udpLayer == nullptr) {
    std::cerr << "Couldn't find TCP nor UDP layers..." << std::endl;
    return std::nullopt;
  }
  if (tcpLayer != nullptr) {
    srcPort = std::to_string(tcpLayer->getSrcPort());
    distPort = std::to_string(tcpLayer->getDstPort());
  } else {
    srcPort = std::to_string(udpLayer->getSrcPort());
    distPort = std::to_string(udpLayer->getDstPort());
  }

  return srcIPAddres + "," + distIPAddres + "," + srcPort + "," + distPort;
}

}; // namespace ps
