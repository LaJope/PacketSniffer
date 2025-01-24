#include <iostream>
#include <memory>

#include "AppSettings.h"
#include "Application.h"
#include "CsvWriter.h"
#include "IReader.h"
#include "IWriter.h"
#include "Logger.h"
#include "NetworkReader.h"
#include "PcapReader.h"

#include "HelpMessage.h"

int main(int argc, char *argv[]) {

  AppSettings settings(argc, argv);

  if (settings.m_help) {
    std::cout << helpMessage << std::endl;
    return 0;
  }
  if (settings.m_verbose) {
    Logger::getInstance().setVerbose(true);
  }

  std::unique_ptr<ps::IPacketWriter> writer;
  std::unique_ptr<ps::IPacketReader> reader;

  if (settings.m_infile) {
    reader = std::make_unique<ps::PcapReader>(settings.m_infile.value());
  } else if (settings.m_deviceName) {
    reader = std::make_unique<ps::NetworkReader>(settings.m_deviceName.value(),
                                                 settings.m_time);
  } else {
    reader = std::make_unique<ps::NetworkReader>(settings.m_time);
  }

  writer = std::make_unique<ps::CsvWriter>(settings.m_outfile);
  Application app(std::move(reader), std::move(writer));
  app.start();

  return 0;
}
