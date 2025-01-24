#pragma once

#include <memory>

#include "IReader.h"
#include "IWriter.h"

class Application {
public:
  Application(std::unique_ptr<ps::IPacketReader>,
              std::unique_ptr<ps::IPacketWriter>);

  void setWriter(std::unique_ptr<ps::IPacketWriter>);
  void setReader(std::unique_ptr<ps::IPacketReader>);

  int start();

private:
  std::shared_ptr<ps::IPacketWriter> m_writer = nullptr;
  std::shared_ptr<ps::IPacketReader> m_reader = nullptr;
};
