#include <iostream>
#include <memory>

#include <pcapplusplus/PcapFileDevice.h>

#include "IWriter.h"
#include "PcapReader.h"

namespace ps {

PcapReader::PcapReader(std::string filename) : m_inputFileName(filename) {}

void PcapReader::Read(std::shared_ptr<IPacketWriter> writer) {
  std::unique_ptr<pcpp::IFileReaderDevice> reader(
      pcpp::IFileReaderDevice::getReader(m_inputFileName));

  if (reader == nullptr) {
    std::cerr << "Cannot determine reader for file type" << std::endl;
    return;
  }

  if (!reader->open()) {
    std::cerr << "Cannot open " << m_inputFileName << " for reading"
              << std::endl;
    return;
  }

  pcpp::RawPacket rawPacket;
  while (reader->getNextPacket(rawPacket)) {
    writer->Write(&rawPacket);
  }

  reader->close();
}

void PcapReader::setInputFileName(std::string fileName) {
  m_inputFileName = fileName;
}

} // namespace ps
