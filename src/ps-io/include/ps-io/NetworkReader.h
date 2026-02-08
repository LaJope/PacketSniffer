#pragma once

#include "IReader.h"

#include <chrono>
#include <memory>
#include <string>

#include "IWriter.h"

namespace ps {

class NetworkReader : public IPacketReader {
public:
  using Callback = std::function<void(pcpp::RawPacket*, pcpp::PcapLiveDevice*, void*)>;

  NetworkReader(uint32_t);
  NetworkReader(std::string, uint32_t);

  int Read(std::shared_ptr<IPacketWriter>) override;

  inline void setDeviceName(std::string device) { m_deviceName = device; }
  inline void setListeningTime(uint32_t time) { m_listeningTime = std::chrono::seconds(time); }

private:
  std::chrono::seconds m_listeningTime;
  std::string m_deviceName;

private:
   Callback getNetworkCallback(std::shared_ptr<IPacketWriter>);
};

} // namespace ps
