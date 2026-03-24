#include <memory>

#include <pcapplusplus/PcapLiveDeviceList.h>
#include <pcapplusplus/SystemUtils.h>

#include <ps-utils/Logger.h>

#include "IWriter.h"
#include "NetworkReader.h"

namespace ps
{

// NetworkReader public

NetworkReader::NetworkReader(uint32_t time)
    : m_deviceName(pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList()[0]->getName())
    , m_listeningTime(time)
{}
NetworkReader::NetworkReader(std::string device, uint32_t time)
    : m_deviceName(device)
    , m_listeningTime(time)
{}

int NetworkReader::Read(std::shared_ptr<IPacketWriter> writer)
{
    pcpp::PcapLiveDevice* device;

    device = pcpp::PcapLiveDeviceList::getInstance().getDeviceByName(m_deviceName);

    if (device == nullptr)
    {
        LOG_ERROR("Cannot find suitable interface");
        return 1;
    }

    if (!device->open())
    {
        LOG_ERROR("Cannot open device '{}'", m_deviceName);
        return 1;
    }

    LOG_INFO("Starting packet capture on interface '{}'...", device->getName());

    device->startCapture(getNetworkCallback(writer), nullptr);

    std::this_thread::sleep_for(std::chrono::seconds(m_listeningTime));
    device->stopCapture();
    device->close();

    LOG_INFO("Capture stopped...");

    return 0;
}

// NetworkReader private

std::function<void(pcpp::RawPacket*, pcpp::PcapLiveDevice*, void*)> NetworkReader::getNetworkCallback(std::shared_ptr<IPacketWriter> writer)
{
    return [&](pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* dev, void* cookie)
    {
        LOG_INFO("Captured a package...");
        writer->Write(rawPacket, dev, cookie);
    };
}

} // namespace ps
