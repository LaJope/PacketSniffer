#include <memory>

#include <pcapplusplus/PcapFileDevice.h>

#include "IWriter.h"
#include "Logger.h"
#include "PcapReader.h"

namespace ps {

PcapReader::PcapReader(std::string filename) : m_inputFileName(filename) {}

int PcapReader::Read(std::shared_ptr<IPacketWriter> writer) {
  std::unique_ptr<pcpp::IFileReaderDevice> reader(
      pcpp::IFileReaderDevice::getReader(m_inputFileName));

  if (reader == nullptr) {
    Logger::getInstance().error("Cannot determine reader for file type");
    return 1;
  }

  if (!reader->open()) {
    Logger::getInstance().error("Cannot open " + m_inputFileName +
                                " for reading");
    return 1;
  }

  Logger::getInstance().log("Starting parsing packets from file " +
                            m_inputFileName + "...");

  pcpp::RawPacket rawPacket;
  while (reader->getNextPacket(rawPacket)) {
    writer->Write(&rawPacket);
  }

  Logger::getInstance().log("Parsing stopped...");

  reader->close();

  return 0;
}

void PcapReader::setInputFileName(std::string fileName) {
  m_inputFileName = fileName;
}

} // namespace ps
