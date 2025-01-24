#include <memory>

#include <pcapplusplus/PcapLiveDeviceList.h>
#include <pcapplusplus/SystemUtils.h>

#include "IWriter.h"
#include "Logger.h"
#include "NetworkReader.h"

namespace ps {

// NetworkReader public

NetworkReader::NetworkReader(uint32_t time)
    : m_deviceName(pcpp::PcapLiveDeviceList::getInstance()
                       .getPcapLiveDevicesList()[0]
                       ->getName()),
      m_listeningTime(time) {}
NetworkReader::NetworkReader(std::string device, uint32_t time)
    : m_deviceName(device), m_listeningTime(time) {}

void NetworkReader::Read(std::shared_ptr<IPacketWriter> writer) {
  pcpp::PcapLiveDevice *device;

  device = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName(
      m_deviceName);

  if (device == nullptr) {
    Logger::getInstance().error("Cannot find suitable interface");
    return;
  }

  if (!device->open()) {
    Logger::getInstance().error("Cannot open device " + m_deviceName);
    return;
  }

  Logger::getInstance().log("Starting packet capture on interface " +
                            device->getName() + "...");

  device->startCapture(getNetworkCallback(writer), nullptr);

  pcpp::multiPlatformSleep(m_listeningTime);

  device->stopCapture();
  device->close();

  Logger::getInstance().log("Capture stopped...");
}

void NetworkReader::setDeviceName(std::string device) { m_deviceName = device; }
void NetworkReader::setListeningTime(uint32_t time) { m_listeningTime = time; }

// NetworkReader private

std::function<void(pcpp::RawPacket *, pcpp::PcapLiveDevice *, void *)>
NetworkReader::getNetworkCallback(std::shared_ptr<IPacketWriter> writer) {
  return [&](pcpp::RawPacket *rawPacket, pcpp::PcapLiveDevice *dev,
             void *cookie) { writer->Write(rawPacket, dev, cookie); };
}

} // namespace ps
