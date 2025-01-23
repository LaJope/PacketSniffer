#include "csvWriter.h"
#include "netInterfaceListener.h"
#include "WriterInterface.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <pcapplusplus/EthLayer.h>
#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/PcapLiveDevice.h>
#include <pcapplusplus/PcapLiveDeviceList.h>

// Callback function to process each packet
void onPacketArrives(pcpp::RawPacket *packet, pcpp::PcapLiveDevice *dev,
                     void *cookie) {

  // Parse the packet
  pcpp::Packet parsedPacket(packet);

  ps::csvPcapWriter h;
  h.Write(packet, dev, cookie);

  // Print packet timestamp and length
  std::cout << "Packet captured! Timestamp: "
            << parsedPacket.getRawPacket()->getPacketTimeStamp().tv_sec
            << ", Length: " << parsedPacket.getRawPacket()->getFrameLength()
            << " bytes" << std::endl;

  // Check if it has an Ethernet layer
  if (parsedPacket.isPacketOfType(pcpp::Ethernet)) {
    pcpp::EthLayer *ethLayer = parsedPacket.getLayerOfType<pcpp::EthLayer>();
    if (ethLayer != nullptr) {
      std::cout << "Ethernet Layer: Src MAC: " << ethLayer->getSourceMac()
                << ", Dst MAC: " << ethLayer->getDestMac() << std::endl;
    }
  }

  // Check if it has an IPv4 layer
  if (parsedPacket.isPacketOfType(pcpp::IPv4)) {
    pcpp::IPv4Layer *ipLayer = parsedPacket.getLayerOfType<pcpp::IPv4Layer>();
    if (ipLayer != nullptr) {
      std::cout << "IPv4 Layer: Src IP: " << ipLayer->getSrcIPAddress()
                << ", Dst IP: " << ipLayer->getDstIPAddress() << std::endl;
    }
  }

  std::cout << "----------------------------------------" << std::endl;
}

int main1() {
  // Get the list of network interfaces
  pcpp::PcapLiveDevice *dev =
      pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList()[0];
  if (dev == nullptr) {
    std::cerr << "Cannot find interface eth0!" << std::endl;
    return 1;
  }

  // Open the device
  if (!dev->open()) {
    std::cerr << "Cannot open device!" << std::endl;
    return 1;
  }

  // Set a filter to capture only TCP packets (optional)
  // dev->setFilter("tcp");
  
  ps::csvPcapWriter wr1;

  ps::IPCAPWriter *wr = &wr1;

  // Start capturing packets
  std::cout << "Starting packet capture on interface " << dev->getName()
            << "..." << std::endl;
  dev->startCapture(ps::netInterfaceListener(wr), nullptr);

  // Keep the program running for 30 seconds
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Stop capturing and close the device
  dev->stopCapture();
  dev->close();

  std::cout << "Capture stopped." << std::endl;
  return 0;
}
