#include <iostream>
#include <memory>

#include <pcapplusplus/PcapLiveDeviceList.h>
#include <pcapplusplus/SystemUtils.h>

#include "IWriter.h"
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
    std::cerr << "Cannot find suitable interface" << std::endl;
    return;
  }

  if (!device->open()) {
    std::cerr << "Cannot open device" << std::endl;
    return;
  }

  std::cout << "Starting packet capture on interface " << device->getName()
            << "..." << std::endl;

  device->startCapture(getNetworkCallback(writer), nullptr);

  pcpp::multiPlatformSleep(m_listeningTime);

  device->stopCapture();
  device->close();

  std::cout << "Capture stopped." << std::endl;
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
