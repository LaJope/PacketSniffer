#pragma once

#include <memory>

#include "IWriter.h"

namespace ps {

class IPacketReader {
public:
  virtual int Read(std::shared_ptr<IPacketWriter> writer) = 0;
};

} // namespace ps
