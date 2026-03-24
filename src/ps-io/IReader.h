#pragma once

#include <memory>

#include "IWriter.h"

namespace ps
{

class IPacketReader
{
  public:
    virtual ~IPacketReader() = default;
    using Ptr = std::shared_ptr<IPacketReader>;

    virtual int Read(IPacketWriter::Ptr writer) = 0;
};

} // namespace ps
