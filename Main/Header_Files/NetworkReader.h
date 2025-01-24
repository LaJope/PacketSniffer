#pragma once

#include <memory>
#include <string>

#include "IReader.h"
#include "IWriter.h"

namespace ps {

class NetworkReader : public IPacketReader {
public:
  NetworkReader(uint32_t);
  NetworkReader(std::string, uint32_t);

  void Read(std::shared_ptr<IPacketWriter>) override;

  void setDeviceName(std::string);
  void setListeningTime(uint32_t);

private:
  uint32_t m_listeningTime;
  std::string m_deviceName;

private:
  std::function<void(pcpp::RawPacket *, pcpp::PcapLiveDevice *, void *)>
      getNetworkCallback(std::shared_ptr<IPacketWriter>);
};

} // namespace ps
