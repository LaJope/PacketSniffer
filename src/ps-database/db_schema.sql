-- Network Packet Database Schema

-- 1. Core Packet Table
CREATE TABLE packets (
    id BIGSERIAL PRIMARY KEY,
    timestamp TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    source_ip INET NOT NULL,
    destination_ip INET NOT NULL,
    source_port SMALLINT,
    destination_port SMALLINT,
    protocol VARCHAR(10) NOT NULL,
    packet_length INTEGER NOT NULL,
    payload BYTEA,
    capture_interface VARCHAR(50),
    vlan_id SMALLINT,

    -- Constraints
    CONSTRAINT chk_source_port CHECK (source_port >= 0 AND source_port <= 65535),
    CONSTRAINT chk_destination_port CHECK (destination_port >= 0 AND destination_port <= 65535),
    CONSTRAINT chk_packet_length CHECK (packet_length > 0),
    CONSTRAINT chk_protocol CHECK (protocol IN ('TCP', 'UDP', 'ICMP', 'ARP', 'OTHER'))
);

-- 2. IP Layer Information Table
CREATE TABLE ip_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(id) ON DELETE CASCADE,
    version SMALLINT NOT NULL,
    header_length SMALLINT NOT NULL,
    type_of_service SMALLINT,
    total_length INTEGER NOT NULL,
    identification INTEGER,
    flags SMALLINT,
    fragment_offset SMALLINT,
    ttl SMALLINT NOT NULL,
    checksum INTEGER,

    -- Constraints
    CONSTRAINT chk_version CHECK (version IN (4, 6)),
    CONSTRAINT chk_ttl CHECK (ttl > 0)
);

-- 3. TCP Layer Information Table
CREATE TABLE tcp_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(id) ON DELETE CASCADE,
    sequence_number INTEGER NOT NULL,
    acknowledgment_number INTEGER,
    header_length SMALLINT NOT NULL,
    flags SMALLINT NOT NULL,
    window_size SMALLINT NOT NULL,
    checksum INTEGER,
    urgent_pointer SMALLINT,

    -- Constraints
    CONSTRAINT chk_flags CHECK (flags >= 0 AND flags <= 255)
);

-- 4. UDP Layer Information Table
CREATE TABLE udp_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(id) ON DELETE CASCADE,
    length SMALLINT NOT NULL,
    checksum INTEGER
);

-- 5. ICMP Layer Information Table
CREATE TABLE icmp_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(id) ON DELETE CASCADE,
    type SMALLINT NOT NULL,
    code SMALLINT NOT NULL,
    checksum INTEGER,
    -- Additional ICMP-specific fields can be added as needed
    -- For example, echo request/reply data, etc.
    icmp_data BYTEA
);

-- 6. ARP Layer Information Table
CREATE TABLE arp_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(id) ON DELETE CASCADE,
    hardware_type SMALLINT NOT NULL,
    protocol_type SMALLINT NOT NULL,
    hardware_length SMALLINT NOT NULL,
    protocol_length SMALLINT NOT NULL,
    operation SMALLINT NOT NULL,
    sender_hardware_address MACADDR NOT NULL,
    sender_protocol_address INET NOT NULL,
    target_hardware_address MACADDR,
    target_protocol_address INET
);

-- 7. Packet Statistics View
CREATE VIEW packet_statistics AS
SELECT
    protocol,
    COUNT(*) as packet_count,
    MIN(timestamp) as first_seen,
    MAX(timestamp) as last_seen,
    COUNT(DISTINCT source_ip) as unique_sources,
    COUNT(DISTINCT destination_ip) as unique_destinations,
    SUM(packet_length) as total_bytes
FROM packets
GROUP BY protocol;

-- 8. Daily Packet Count View
CREATE VIEW daily_packet_counts AS
SELECT
    DATE(timestamp) as date,
    protocol,
    COUNT(*) as packet_count,
    SUM(packet_length) as total_bytes
FROM packets
GROUP BY DATE(timestamp), protocol
ORDER BY date DESC;

-- 9. Top Talkers View
CREATE VIEW top_talkers AS
SELECT
    source_ip,
    destination_ip,
    protocol,
    COUNT(*) as packet_count,
    SUM(packet_length) as total_bytes,
    MIN(timestamp) as first_seen,
    MAX(timestamp) as last_seen
FROM packets
GROUP BY source_ip, destination_ip, protocol
ORDER BY packet_count DESC
LIMIT 100;

-- 10. Indexes for Performance
CREATE INDEX idx_packets_timestamp ON packets(timestamp);
CREATE INDEX idx_packets_source_ip ON packets(source_ip);
CREATE INDEX idx_packets_destination_ip ON packets(destination_ip);
CREATE INDEX idx_packets_protocol ON packets(protocol);
CREATE INDEX idx_packets_source_port ON packets(source_port);
CREATE INDEX idx_packets_destination_port ON packets(destination_port);
CREATE INDEX idx_packets_length ON packets(packet_length);
CREATE INDEX idx_packets_capture_interface ON packets(capture_interface);

-- 11. Functions for Data Management

-- Function to clean old packets (older than X days)
CREATE OR REPLACE FUNCTION cleanup_old_packets(days_old INTEGER DEFAULT 30)
RETURNS INTEGER AS $$
DECLARE
    rows_deleted INTEGER;
BEGIN
    DELETE FROM packets
    WHERE timestamp < NOW() - INTERVAL '1 day' * days_old;
    GET DIAGNOSTICS rows_deleted = ROW_COUNT;
    RETURN rows_deleted;
END;
$$ LANGUAGE plpgsql;

-- Function to get packet count by date range
CREATE OR REPLACE FUNCTION get_packet_count_by_date_range(
    start_date TIMESTAMP WITH TIME ZONE,
    end_date TIMESTAMP WITH TIME ZONE,
    protocol_filter VARCHAR(10) DEFAULT NULL
)
RETURNS TABLE(
    protocol VARCHAR(10),
    count BIGINT,
    total_bytes BIGINT
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        p.protocol,
        COUNT(*)::BIGINT,
        SUM(p.packet_length)::BIGINT
    FROM packets p
    WHERE p.timestamp BETWEEN start_date AND end_date
        AND (protocol_filter IS NULL OR p.protocol = protocol_filter)
    GROUP BY p.protocol;
END;
$$ LANGUAGE plpgsql;

-- Function to get top protocols by byte count
CREATE OR REPLACE FUNCTION get_top_protocols_by_bytes(limit_count INTEGER DEFAULT 10)
RETURNS TABLE(
    protocol VARCHAR(10),
    packet_count BIGINT,
    total_bytes BIGINT
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        p.protocol,
        COUNT(*)::BIGINT,
        SUM(p.packet_length)::BIGINT
    FROM packets p
    GROUP BY p.protocol
    ORDER BY SUM(p.packet_length) DESC
    LIMIT limit_count;
END;
$$ LANGUAGE plpgsql;

-- 12. Triggers for Automatic Updates

-- Trigger function to update timestamps when packet data changes
CREATE OR REPLACE FUNCTION update_modified_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.timestamp = NOW();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Note: Since we're mainly dealing with inserts, we don't need a trigger for updates
-- But we can add one for future expansion if needed

-- 13. Sample Data Insertion Function
CREATE OR REPLACE FUNCTION insert_simple_packet(
    src_ip INET,
    dst_ip INET,
    src_port SMALLINT,
    dst_port SMALLINT,
    proto VARCHAR(10),
    length INTEGER,
    payload BYTEA DEFAULT NULL,
    interface VARCHAR(50) DEFAULT NULL
)
RETURNS BIGINT AS $$
DECLARE
    packet_id BIGINT;
BEGIN
    INSERT INTO packets (
        source_ip, destination_ip, source_port, destination_port,
        protocol, packet_length, payload, capture_interface
    ) VALUES (
        src_ip, dst_ip, src_port, dst_port,
        proto, length, payload, interface
    ) RETURNING id INTO packet_id;

    RETURN packet_id;
END;
$$ LANGUAGE plpgsql;

-- 14. Procedures for Bulk Operations

-- Procedure to bulk insert packets (can be extended for actual packet processing)
CREATE OR REPLACE PROCEDURE bulk_insert_packets(
    packet_data JSONB[]
)
LANGUAGE plpgsql
AS $$
DECLARE
    pkt JSONB;
BEGIN
    FOREACH pkt IN ARRAY packet_data
    LOOP
        INSERT INTO packets (
            source_ip, destination_ip, source_port, destination_port,
            protocol, packet_length, payload, capture_interface
        ) VALUES (
            (pkt->>'source_ip')::INET,
            (pkt->>'destination_ip')::INET,
            (pkt->>'source_port')::SMALLINT,
            (pkt->>'destination_port')::SMALLINT,
            pkt->>'protocol',
            (pkt->>'packet_length')::INTEGER,
            (pkt->>'payload')::BYTEA,
            (pkt->>'capture_interface')
        );
    END LOOP;
END;
$$;

-- Procedure to create protocol-specific entries for a packet
CREATE OR REPLACE PROCEDURE create_protocol_entries(
    packet_id BIGINT,
    protocol_data JSONB
)
LANGUAGE plpgsql
AS $$
DECLARE
    protocol VARCHAR(10);
BEGIN
    protocol := protocol_data->>'protocol';

    IF protocol = 'TCP' THEN
        INSERT INTO tcp_packets (
            packet_id, sequence_number, acknowledgment_number, header_length,
            flags, window_size, checksum, urgent_pointer
        ) VALUES (
            packet_id,
            (protocol_data->>'sequence_number')::INTEGER,
            (protocol_data->>'acknowledgment_number')::INTEGER,
            (protocol_data->>'header_length')::SMALLINT,
            (protocol_data->>'flags')::SMALLINT,
            (protocol_data->>'window_size')::SMALLINT,
            (protocol_data->>'checksum')::INTEGER,
            (protocol_data->>'urgent_pointer')::SMALLINT
        );
    ELSIF protocol = 'UDP' THEN
        INSERT INTO udp_packets (
            packet_id, length, checksum
        ) VALUES (
            packet_id,
            (protocol_data->>'length')::SMALLINT,
            (protocol_data->>'checksum')::INTEGER
        );
    ELSIF protocol = 'ICMP' THEN
        INSERT INTO icmp_packets (
            packet_id, type, code, checksum, icmp_data
        ) VALUES (
            packet_id,
            (protocol_data->>'type')::SMALLINT,
            (protocol_data->>'code')::SMALLINT,
            (protocol_data->>'checksum')::INTEGER,
            (protocol_data->>'icmp_data')::BYTEA
        );
    ELSIF protocol = 'ARP' THEN
        INSERT INTO arp_packets (
            packet_id, hardware_type, protocol_type, hardware_length,
            protocol_length, operation, sender_hardware_address,
            sender_protocol_address, target_hardware_address,
            target_protocol_address
        ) VALUES (
            packet_id,
            (protocol_data->>'hardware_type')::SMALLINT,
            (protocol_data->>'protocol_type')::SMALLINT,
            (protocol_data->>'hardware_length')::SMALLINT,
            (protocol_data->>'protocol_length')::SMALLINT,
            (protocol_data->>'operation')::SMALLINT,
            (protocol_data->>'sender_hardware_address')::MACADDR,
            (protocol_data->>'sender_protocol_address')::INET,
            (protocol_data->>'target_hardware_address')::MACADDR,
            (protocol_data->>'target_protocol_address')::INET
        );
    END IF;
END;
$$;

-- Universal procedure to handle packet insertion with protocol data
CREATE OR REPLACE PROCEDURE insert_packet_with_protocol_data(
    packet_data JSONB,
    protocol_data JSONB DEFAULT NULL
)
LANGUAGE plpgsql
AS $$
DECLARE
    packet_id BIGINT;
BEGIN
    -- Insert the base packet
    INSERT INTO packets (
        source_ip, destination_ip, source_port, destination_port,
        protocol, packet_length, payload, capture_interface
    ) VALUES (
        (packet_data->>'source_ip')::INET,
        (packet_data->>'destination_ip')::INET,
        (packet_data->>'source_port')::SMALLINT,
        (packet_data->>'destination_port')::SMALLINT,
        packet_data->>'protocol',
        (packet_data->>'packet_length')::INTEGER,
        (packet_data->>'payload')::BYTEA,
        (packet_data->>'capture_interface')
    ) RETURNING id INTO packet_id;

    IF protocol_data IS NOT NULL THEN
        PERFORM create_protocol_entries(packet_id, protocol_data);
    END IF;
END;
$$;

-- Procedure to bulk insert packets with protocol data
CREATE OR REPLACE PROCEDURE bulk_insert_packets_with_protocol_data(
    packet_data_list JSONB[]
)
LANGUAGE plpgsql
AS $$
DECLARE
    pkt JSONB;
BEGIN
    FOREACH pkt IN ARRAY packet_data_list
    LOOP
        CALL insert_packet_with_protocol_data(
            pkt->'packet_data',
            pkt->'protocol_data'
        );
    END LOOP;
END;
$$;
