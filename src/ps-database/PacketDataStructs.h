#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

namespace ps
{
using json = nlohmann::json;

struct PacketData
{
    using Ptr = std::shared_ptr<PacketData>;

    std::string protocol;
    std::string payload;
    std::string capture_interface;
    size_t packet_length;
    int vlan_id;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PacketData, protocol, packet_length, payload, capture_interface, vlan_id);
};

struct IPPacketData
{
    using Ptr = std::shared_ptr<IPPacketData>;

    size_t header_length;
    uint32_t source_ip;
    uint32_t destination_ip;
    uint16_t total_length;
    uint16_t fragment_offset;
    uint16_t checksum;
    uint16_t identification;
    uint8_t version;
    uint8_t type_of_service;
    uint8_t ttl;
    uint8_t flags;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        IPPacketData, version, header_length, type_of_service, total_length, identification, flags, fragment_offset, ttl, checksum,
        source_ip, destination_ip);
};

struct TCPPacketData
{
    using Ptr = std::shared_ptr<TCPPacketData>;

    size_t header_length;
    uint32_t sequence_number;
    uint32_t acknowledgment_number;
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_pointer;
    uint8_t fin_flag : 1;
    uint8_t syn_flag : 1;
    uint8_t rst_flag : 1;
    uint8_t psh_flag : 1;
    uint8_t ack_flag : 1;
    uint8_t urg_flag : 1;
    uint8_t ece_flag : 1;
    uint8_t cwr_flag : 1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        TCPPacketData, sequence_number, acknowledgment_number, source_port, destination_port, header_length, window_size, checksum,
        urgent_pointer, fin_flag, syn_flag, rst_flag, psh_flag, ack_flag, urg_flag, ece_flag, cwr_flag);
};

struct UDPPacketData
{
    using Ptr = std::shared_ptr<UDPPacketData>;

    size_t header_length;
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t checksum;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UDPPacketData, header_length, source_port, destination_port, checksum);
};

struct ICMPPacketData
{
    using Ptr = std::shared_ptr<ICMPPacketData>;

    std::string icmp_data;
    uint16_t checksum;
    uint8_t type;
    uint8_t code;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ICMPPacketData, type, code, checksum, icmp_data);
};

struct ARPPacketData
{
    using Ptr = std::shared_ptr<ARPPacketData>;

    uint8_t sender_hardware_address[6];
    uint8_t target_hardware_address[6];
    uint32_t sender_protocol_address;
    uint32_t target_protocol_address;
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint16_t operation;
    uint8_t hardware_length;
    uint8_t protocol_length;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        ARPPacketData, hardware_type, protocol_type, hardware_length, protocol_length, operation, sender_hardware_address,
        sender_protocol_address, target_hardware_address, target_protocol_address);
};

struct FullPacket
{
    using Ptr = std::shared_ptr<FullPacket>;

    PacketData::Ptr packetData = nullptr;
    IPPacketData::Ptr ipPacketData = nullptr;
    TCPPacketData::Ptr tcpPacketData = nullptr;
    UDPPacketData::Ptr udpPacketData = nullptr;
    ICMPPacketData::Ptr icmpPacketData = nullptr;
    ARPPacketData::Ptr arpPacketData = nullptr;
};

} // namespace ps
