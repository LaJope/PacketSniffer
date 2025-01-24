#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include <pcapplusplus/EthLayer.h>
#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/PcapFileDevice.h>
#include <pcapplusplus/PcapLiveDevice.h>
#include <pcapplusplus/PcapLiveDeviceList.h>
#include <pcapplusplus/SystemUtils.h>

#include "Application.h"
#include "IWriter.h"
#include "netInterfaceListener.h"

// Application public

void Application::setWriter(std::unique_ptr<ps::IPacketWriter> writer) {
  m_writer = std::move(writer);
}

void Application::setDevice(std::string device) { m_deviceName = device; }
void Application::setTimeForListening(uint64_t time) { m_listeningTime = time; }

void Application::setInputPcapFile(std::string file) {
  m_listenOnDevice = false;
  m_inputFile = file;
}
void Application::setOutputFile(std::string file) { m_outputFile = file; }

int Application::start() {
  if (m_listenOnDevice)
    return startListeningToDevice();
  return startReadingFromFile();
}

// Application private

int Application::startReadingFromFile() {
  std::unique_ptr<pcpp::IFileReaderDevice> reader(
      pcpp::IFileReaderDevice::getReader(m_inputFile));

  if (reader == nullptr) {
    std::cerr << "Cannot determine reader for file type" << std::endl;
    return 1;
  }

  if (!reader->open()) {
    std::cerr << "Cannot open " << m_inputFile << " for reading" << std::endl;
    return 1;
  }

  pcpp::RawPacket rawPacket;
  while (reader->getNextPacket(rawPacket)) {
    m_writer->Write(&rawPacket);
  }

  reader->close();

  return 0;
}

int Application::startListeningToDevice() {
}
