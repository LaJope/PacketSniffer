#include "WriterInterface.h"
#include "csvWriter.h"

#include <iostream>
#include <memory>

#include <pcapplusplus/PcapFileDevice.h>
#include <pcapplusplus/SystemUtils.h>

int main(int argc, char *argv[]) {
  std::string filename = std::string(argv[1]);
  std::unique_ptr<pcpp::IFileReaderDevice> reader(
      pcpp::IFileReaderDevice::getReader(filename));

  if (reader == nullptr) {
    std::cerr << "Cannot determine reader for file type" << std::endl;
    return 1;
  }

  if (!reader->open()) {
    std::cerr << "Cannot open input.pcap for reading" << std::endl;
    return 1;
  }

  pcpp::RawPacket rawPacket;
  std::unique_ptr<ps::IPCAPWriter> writer =
      std::make_unique<ps::csvPcapWriter>();

  // a while loop that will continue as long as there are packets in the input
  // file matching the BPF filter
  while (reader->getNextPacket(rawPacket)) {
    writer->Write(&rawPacket);
  }

  pcpp::multiPlatformSleep(10);

  reader->close();

  return 0;
}
