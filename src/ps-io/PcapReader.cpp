#include <memory>

#include <pcapplusplus/PcapFileDevice.h>
#include <ps-utils/Logger.h>

#include "IWriter.h"
#include "PcapReader.h"

namespace ps
{

PcapReader::PcapReader(std::string filename)
    : m_inputFileName(filename)
{}

int PcapReader::Read(std::shared_ptr<IPacketWriter> writer)
{
    auto fileReader(pcpp::IFileReaderDevice::getReader(m_inputFileName));

    if (!fileReader)
    {
        LOG_ERROR("Cannot determine reader for file type");
        return 1;
    }

    if (!fileReader->open())
    {
        LOG_ERROR("Cannot open '{}' for reading", m_inputFileName);
        return 1;
    }

    LOG_INFO("Starting parsing packets from file '{}'...", m_inputFileName);

    pcpp::RawPacket rawPacket;
    while (fileReader->getNextPacket(rawPacket))
    {
        writer->Write(&rawPacket);
    }

    LOG_INFO("Parsing stopped...");

    fileReader->close();

    return 0;
}

void PcapReader::setInputFileName(std::string fileName)
{
    m_inputFileName = fileName;
}

} // namespace ps
