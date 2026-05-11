#include "DatabaseWriter.h"
#include "ps-database/PacketDatabaseManager.h"

#include <PcapFilter.h>
#include <ProtocolType.h>
#include <SystemUtils.h>
#include <arpa/inet.h>

#include <iostream>
#include <memory>

#include <pcapplusplus/ArpLayer.h>
#include <pcapplusplus/IPv4Layer.h>
#include <pcapplusplus/IcmpLayer.h>
#include <pcapplusplus/Layer.h>
#include <pcapplusplus/Packet.h>
#include <pcapplusplus/ProtocolType.h>
#include <pcapplusplus/TcpLayer.h>
#include <pcapplusplus/UdpLayer.h>
#include <pcapplusplus/VlanLayer.h>

#include <ps-database/PacketDataStructs.h>
#include <ps-utils/Logger.h>

namespace ps
{
DatabaseWriter::DatabaseWriter()
    : m_packetDatabaseManager()
{}
DatabaseWriter::DatabaseWriter(std::string connectionParameters)
    : m_packetDatabaseManager()
{}

void DatabaseWriter::Write(pcpp::RawPacket* rawPacket, pcpp::PcapLiveDevice* device, void* misc)
{
    auto packet = pcpp::Packet{rawPacket};
    if (!packet.isPacketOfType(pcpp::IPv4))
    {
        LOG_WARNING("Packet is not of IPv4 type");
        return;
    }

    const auto parsedData = parseFullPacket(packet, device->getName());

    // json packetData{*parsedData->packetData};
    // json protocolData;
    //
    // if (parsedData->ipPacketData)
    //     protocolData["IP"] = json{*parsedData->ipPacketData}[0];
    // if (parsedData->tcpPacketData)
    //     protocolData["TCP"] = json{*parsedData->tcpPacketData}[0];
    // if (parsedData->udpPacketData)
    //     protocolData["UDP"] = json{*parsedData->udpPacketData}[0];
    // if (parsedData->icmpPacketData)
    //     protocolData["ICMP"] = json{*parsedData->icmpPacketData}[0];
    // if (parsedData->arpPacketData)
    //     protocolData["ARP"] = json{*parsedData->arpPacketData}[0];
    //
    // std::cout << packetData[0].dump(4) << std::endl << std::endl;
    // std::cout << protocolData.dump(4) << std::endl << std::endl;

    auto ok = m_packetDatabaseManager->insertPacketWithProtocolData(parsedData);

    if (!ok)
        LOG_ERROR("Could not write packet data to database");
}

void DatabaseWriter::Flush()
{}

FullPacket::Ptr DatabaseWriter::parseFullPacket(const pcpp::Packet& packet, const std::string& devName)
{
    return std::make_shared<FullPacket>(FullPacket{
        .packetData = parsePacketData(packet, devName),
        .ipPacketData = parsePacketIPData(packet),
        .tcpPacketData = parsePacketTCPData(packet),
        .udpPacketData = parsePacketUDPData(packet),
        .icmpPacketData = parsePacketICMPData(packet),
        .arpPacketData = parsePacketARPData(packet),
    });
}

PacketData::Ptr DatabaseWriter::parsePacketData(const pcpp::Packet& packet, const std::string& devName)
{
    std::string payload = packet.toString();

    auto vlanLayer = packet.getLayerOfType<pcpp::VlanLayer>();
    auto vlan_id = vlanLayer ? vlanLayer->getVlanID() : 0;

    std::string protocol;
    if (packet.isPacketOfType(pcpp::TCP))
        protocol = "TCP";
    else if (packet.isPacketOfType(pcpp::UDP))
        protocol = "UDP";
    else if (packet.isPacketOfType(pcpp::ICMP))
        protocol = "ICMP";
    else if (packet.isPacketOfType(pcpp::ARP))
        protocol = "ARP";
    else
        protocol = "OTHER";

    // TODO: ntohs/l !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    return std::make_shared<PacketData>(PacketData{
        .protocol = protocol,
        .payload = payload,
        .capture_interface = devName,
        .packet_length = payload.length(),
        .vlan_id = vlan_id,
    });
}

IPPacketData::Ptr DatabaseWriter::parsePacketIPData(const pcpp::Packet& packet)
{
    const auto ipv4Layer = packet.getLayerOfType<pcpp::IPv4Layer>();
    if (!ipv4Layer)
        return nullptr;

    const auto ipv4Header = ipv4Layer->getIPv4Header();
    if (!ipv4Header)
        return nullptr;

    uint16_t fragmentOffset = ipv4Header->fragmentOffset;
    uint8_t flags = (fragmentOffset >> 13) & 0x07;

    return std::make_shared<IPPacketData>(IPPacketData{
        .header_length = ipv4Layer->getHeaderLen(),
        .source_ip = ipv4Header->ipSrc,
        .destination_ip = ipv4Header->ipDst,
        .total_length = ipv4Header->totalLength,
        .fragment_offset = fragmentOffset,
        .checksum = ipv4Header->headerChecksum,
        .identification = ipv4Header->ipId,
        .version = ipv4Header->ipVersion,
        .type_of_service = ipv4Header->typeOfService,
        .ttl = ipv4Header->timeToLive,
        .flags = flags,
    });
}

TCPPacketData::Ptr DatabaseWriter::parsePacketTCPData(const pcpp::Packet& packet)
{
    const auto tcpLayer = packet.getLayerOfType<pcpp::TcpLayer>();
    if (!tcpLayer)
        return nullptr;

    const auto tcpHeader = tcpLayer->getTcpHeader();
    if (!tcpHeader)
        return nullptr;

    return std::make_shared<TCPPacketData>(TCPPacketData{
        .header_length = tcpLayer->getHeaderLen(),
        .sequence_number = tcpHeader->sequenceNumber,
        .acknowledgment_number = tcpHeader->ackNumber,
        .source_port = tcpHeader->portSrc,
        .destination_port = tcpHeader->portDst,
        .window_size = tcpHeader->windowSize,
        .checksum = tcpHeader->headerChecksum,
        .urgent_pointer = tcpHeader->urgentPointer,
        .fin_flag = tcpHeader->finFlag,
        .syn_flag = tcpHeader->synFlag,
        .rst_flag = tcpHeader->rstFlag,
        .psh_flag = tcpHeader->pshFlag,
        .ack_flag = tcpHeader->ackFlag,
        .urg_flag = tcpHeader->urgFlag,
        .ece_flag = tcpHeader->eceFlag,
        .cwr_flag = tcpHeader->cwrFlag,
    });
}

UDPPacketData::Ptr DatabaseWriter::parsePacketUDPData(const pcpp::Packet& packet)
{
    const auto udpLayer = packet.getLayerOfType<pcpp::UdpLayer>();
    if (!udpLayer)
        return nullptr;

    const auto udpHeader = udpLayer->getUdpHeader();
    if (!udpHeader)
        return nullptr;

    return std::make_shared<UDPPacketData>(UDPPacketData{
        .header_length = udpLayer->getHeaderLen(),
        .source_port = udpHeader->portSrc,
        .destination_port = udpHeader->portDst,
        .checksum = udpHeader->headerChecksum,
    });
}

ICMPPacketData::Ptr DatabaseWriter::parsePacketICMPData(const pcpp::Packet& packet)
{
    const auto icmpLayer = packet.getLayerOfType<pcpp::IcmpLayer>();
    if (!icmpLayer)
        return nullptr;

    const auto icmpHeader = icmpLayer->getIcmpHeader();
    if (!icmpHeader)
        return nullptr;

    auto icmp_data_len = icmpLayer->getDataLen();

    return std::make_shared<ICMPPacketData>(ICMPPacketData{
        .icmp_data = std::string(reinterpret_cast<const char*>(icmpLayer->getData()), icmp_data_len),
        .checksum = icmpHeader->checksum,
        .type = icmpHeader->type,
        .code = icmpHeader->code,
    });
}

ARPPacketData::Ptr DatabaseWriter::parsePacketARPData(const pcpp::Packet& packet)
{
    const auto arpLayer = packet.getLayerOfType<pcpp::ArpLayer>();
    if (!arpLayer)
        return nullptr;

    const auto arpHeader = arpLayer->getArpHeader();
    if (!arpHeader)
        return nullptr;

    auto arpData = std::make_shared<ARPPacketData>(ARPPacketData{
        .sender_protocol_address = arpHeader->senderIpAddr,
        .target_protocol_address = arpHeader->targetIpAddr,
        .hardware_type = arpHeader->hardwareType,
        .protocol_type = arpHeader->protocolType,
        .operation = arpHeader->opcode,
        .hardware_length = arpHeader->hardwareSize,
        .protocol_length = arpHeader->protocolSize,
    });

    std::memcpy(&arpData->sender_hardware_address, arpHeader->senderMacAddr, 6);
    std::memcpy(&arpData->target_hardware_address, arpHeader->targetMacAddr, 6);

    return arpData;
}
} // namespace ps
