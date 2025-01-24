#include <iostream>
#include <memory>

#include <pcapplusplus/EthLayer.h>
#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/PcapLiveDevice.h>
#include <pcapplusplus/PcapLiveDeviceList.h>

#include "Application.h"
#include "csvWriter.h"

int main() {
  Application app;
  std::string filename = "./pcapfiles/4SICS-GeekLounge-151020.pcap";
  app.setInputPcapFile(filename);
  app.setWriter(std::make_unique<ps::csvPcapWriter>(filename));
  // app.setTimeForListening(20);
  app.start();

  return 0;
}
