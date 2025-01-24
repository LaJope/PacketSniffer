#pragma once

#include <map>
#include <optional>
#include <utility>

#include "IWriter.h"

namespace ps {

class csvPcapWriter : public IPacketWriter {
public:
  csvPcapWriter(std::string);

  void Write(pcpp::RawPacket *, pcpp::PcapLiveDevice * = nullptr,
             void * = nullptr) override;

private:
  std::map<std::string, std::pair<int, int>> m_data;

  std::string m_outputFile;

private:
  std::optional<std::string> getDataKey(const pcpp::Packet &) const;
};

} // namespace ps
