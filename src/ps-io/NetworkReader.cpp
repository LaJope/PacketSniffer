#include "NetworkReader.h"

#include <memory>

#include <pcapplusplus/PcapLiveDeviceList.h>
#include <pcapplusplus/SystemUtils.h>

#include <ps-utils/Logger.h>

#include "IWriter.h"

namespace ps {

// NetworkReader public

NetworkReader::NetworkReader(uint32_t time)
    : m_deviceName(pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList()[0]->getName()),
      m_listeningTime(std::chrono::seconds(time)) {}
NetworkReader::NetworkReader(std::string device, uint32_t time) : m_deviceName(device), m_listeningTime(time) {}

int NetworkReader::Read(std::shared_ptr<IPacketWriter> writer) {
  pcpp::PcapLiveDevice* device;

  device = pcpp::PcapLiveDeviceList::getInstance().getDeviceByName(m_deviceName);

  if (device == nullptr) {
    Logger::getInstance().error("Cannot find suitable interface");
    return 1;
  }

  if (!device->open()) {
    Logger::getInstance().error("Cannot open device " + m_deviceName);
    return 1;
  }

  Logger::getInstance().log("Starting packet capture on interface " + device->getName() + "...");

  device->startCapture(getNetworkCallback(writer), nullptr);

  std::this_thread::sleep_for(m_listeningTime);

  device->stopCapture();
  device->close();

  Logger::getInstance().log("Capture stopped...");

  return 0;
}

NetworkReader::Callback NetworkReader::getNetworkCallback(std::shared_ptr<IPacketWriter> writer) {
  return [&](pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* dev, void* cookie) {
    Logger::getInstance().log("Captured a package...");
    writer->Write(rawPacket, dev, cookie);
  };
}

} // namespace ps
