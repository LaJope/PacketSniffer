#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "IWriter.h"
#include "IReader.h"

class Application {
public:
  void setWriter(std::unique_ptr<ps::IPacketWriter> writer);

  void setDevice(std::string device);
  void setTimeForListening(uint64_t time);

  void setInputPcapFile(std::string fileName);
  void setOutputFile(std::string fileName);

  int start();

private:
  bool m_listenOnDevice = true;

  std::shared_ptr<ps::IPacketWriter> m_writer = nullptr;
  std::shared_ptr<ps::IPacketReader> m_reader = nullptr;

  std::string m_inputFile;
  std::string m_outputFile;

  std::string m_deviceName;
  uint64_t m_listeningTime = 60;

private:
  int startReadingFromFile();

  int startListeningToDevice();
};
