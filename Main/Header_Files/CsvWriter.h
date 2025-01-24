#pragma once

#include <map>
#include <optional>
#include <utility>

#include "IWriter.h"

namespace ps {

class CsvWriter : public IPacketWriter {
public:
  CsvWriter(std::string = "PcapIPInfo");

  void Write(pcpp::RawPacket *, pcpp::PcapLiveDevice * = nullptr,
             void * = nullptr) override;

  void Flush() override;

private:
  std::map<std::string, std::pair<int, int>> m_data;

  std::string m_outputFileName;

private:
  std::optional<std::string> getDataKey(const pcpp::Packet &) const;
};

} // namespace ps
