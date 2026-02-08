#include "Application.h"

#include <memory>

#include <ps-io/IReader.h>
#include <ps-io/IWriter.h>

#include <ps-utils/Logger.h>

Application::Application(std::unique_ptr<ps::IPacketReader> reader,
                         std::unique_ptr<ps::IPacketWriter> writer)
    : m_reader(std::move(reader)), m_writer(std::move(writer)) {}

void Application::setWriter(std::unique_ptr<ps::IPacketWriter> writer) {
  m_writer = std::move(writer);
}

void Application::setReader(std::unique_ptr<ps::IPacketReader> reader) {
  m_reader = std::move(reader);
}

int Application::start() {
  if (m_reader == nullptr || m_writer == nullptr) {
    Logger::getInstance().error("Either Reader or Writer (or both) is not "
                                "initialized for PacketSniffer. Aborting...");
    return 1;
  }
  if (m_reader->Read(m_writer))
    return 1;

  m_writer->Flush();
  return 0;
}
