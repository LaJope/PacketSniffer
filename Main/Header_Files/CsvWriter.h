#pragma once

#include <mutex>
#include <optional>
#include <unordered_map>
#include <utility>

#include "IWriter.h"

namespace ps {

class CsvWriter : public IPacketWriter {
public:
  CsvWriter(std::string);

  void Write(pcpp::RawPacket *, pcpp::PcapLiveDevice * = nullptr,
             void * = nullptr) override;

  void Flush() override;

private:
  std::unordered_map<std::string, std::pair<int, int>> m_data;

  std::string m_outputFileName;

  std::mutex m_lock;


private:
  std::optional<std::string> getDataKey(const pcpp::Packet &) const;
};

} // namespace ps
