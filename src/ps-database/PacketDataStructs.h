#pragma once

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

    std::string source_ip;
    std::string destination_ip;
    int source_port;
    int destination_port;
    std::string protocol;
    int packet_length;
    std::string payload;
    std::string capture_interface;
    int vlan_id;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        PacketData, source_ip, destination_ip, source_port, destination_port, protocol, packet_length, payload, capture_interface, vlan_id);
};

struct IPPacketData
{
    using Ptr = std::shared_ptr<IPPacketData>;

    int version;
    char header_length;
    char type_of_service;
    int total_length;
    int identification;
    char flags;
    int fragment_offset;
    int ttl;
    int checksum;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        IPPacketData, version, header_length, type_of_service, total_length, identification, flags, fragment_offset, ttl, checksum);
};

struct TCPPacketData
{
    using Ptr = std::shared_ptr<TCPPacketData>;

    int sequence_number;
    int acknowledgment_number;
    int header_length;
    int flags;
    int window_size;
    int checksum;
    int urgent_pointer;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(
        TCPPacketData, sequence_number, acknowledgment_number, header_length, flags, window_size, checksum, urgent_pointer);
};

struct UDPPacketData
{
    using Ptr = std::shared_ptr<UDPPacketData>;

    int length;
    int checksum;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UDPPacketData, length, checksum);
};

struct ICMPPacketData
{
    using Ptr = std::shared_ptr<ICMPPacketData>;

    int type;
    int code;
    int checksum;
    std::string icmp_data;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ICMPPacketData, type, code, checksum, icmp_data);
};

struct ARPPacketData
{
    using Ptr = std::shared_ptr<ARPPacketData>;

    int hardware_type;
    int protocol_type;
    int hardware_length;
    int protocol_length;
    int operation;
    std::string sender_hardware_address;
    std::string sender_protocol_address;
    std::string target_hardware_address;
    std::string target_protocol_address;

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

    FullPacket() = default;
    ~FullPacket() = default;
};

} // namespace ps
