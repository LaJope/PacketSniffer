#include <memory>

#include <pcapplusplus/EthLayer.h>
#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/PcapLiveDevice.h>
#include <pcapplusplus/PcapLiveDeviceList.h>

#include "Application.h"
#include "CsvWriter.h"
#include "IReader.h"
#include "IWriter.h"
#include "NetworkReader.h"
#include "PcapReader.h"

int main(int argc, char *argv[]) {
  std::unique_ptr<ps::IPacketReader> reader;
  std::unique_ptr<ps::IPacketWriter> writer;
  // std::unique_ptr<ps::IPacketReader> reader =
  // std::make_unique<ps::PcapReader>(
  //     "./pcapfiles/input.pcap");
  reader = std::make_unique<ps::NetworkReader>(5);
  writer = std::make_unique<ps::CsvWriter>("output");
  Application app(std::move(reader), std::move(writer));
  app.start();

  return 0;
}
