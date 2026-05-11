-- 
-- Посмотреть, что быстрее CONSTRAINT или пользовательский класс ENUM
-- Сделать ON UPDATE RESTRICT
-- Добавить первичные/вторичные ключи в таблицы (первичный -> вторичный, +первичный)
-- 

-- TABLES

CREATE TABLE packets (
    packet_id BIGSERIAL PRIMARY KEY,
    timestamp TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    protocol VARCHAR(10) NOT NULL,
    payload BYTEA,
    capture_interface VARCHAR(50),
    packet_length INTEGER NOT NULL,
    vlan_id SMALLINT,

    CONSTRAINT chk_packet_length CHECK (packet_length > 0),
    CONSTRAINT chk_protocol CHECK (protocol IN ('TCP', 'UDP', 'ICMP', 'ARP', 'OTHER'))
);

CREATE TABLE ip_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(packet_id) ON DELETE CASCADE,
    version SMALLINT NOT NULL,
    header_length SMALLINT NOT NULL,
    type_of_service SMALLINT,
    source_ip INET NOT NULL,
    destination_ip INET NOT NULL,
    total_length INTEGER NOT NULL,
    identification INTEGER,
    flags SMALLINT NOT NULL,
    fragment_offset SMALLINT,
    ttl INTEGER NOT NULL,
    checksum INTEGER,

    CONSTRAINT chk_version CHECK (version IN (4, 6)),
    CONSTRAINT chk_ttl CHECK (ttl > 0)
);

CREATE TABLE tcp_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(packet_id) ON DELETE CASCADE,
    sequence_number BIGINT NOT NULL,
    acknowledgment_number BIGINT,
    source_port INTEGER NOT NULL,
    destination_port INTEGER NOT NULL,
    header_length INTEGER NOT NULL,
    fin_flag SMALLINT,
    syn_flag SMALLINT,
    rst_flag SMALLINT,
    psh_flag SMALLINT,
    ack_flag SMALLINT,
    urg_flag SMALLINT,
    ece_flag SMALLINT,
    cwr_flag SMALLINT,
    window_size INTEGER NOT NULL,
    checksum INTEGER,
    urgent_pointer SMALLINT,

    CONSTRAINT chk_source_port CHECK (source_port >= 0 AND source_port <= 65535),
    CONSTRAINT chk_destination_port CHECK (destination_port >= 0 AND destination_port <= 65535)
);

CREATE TABLE udp_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(packet_id) ON DELETE CASCADE,
    length INTEGER NOT NULL,
    source_port INTEGER NOT NULL,
    destination_port INTEGER NOT NULL,
    checksum INTEGER

    CONSTRAINT chk_source_port CHECK (source_port >= 0 AND source_port <= 65535),
    CONSTRAINT chk_destination_port CHECK (destination_port >= 0 AND destination_port <= 65535)
);

CREATE TABLE icmp_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(packet_id) ON DELETE CASCADE,
    type SMALLINT NOT NULL,
    code SMALLINT NOT NULL,
    checksum INTEGER,
    icmp_data BYTEA
);

CREATE TABLE arp_packets (
    packet_id BIGINT PRIMARY KEY REFERENCES packets(packet_id) ON DELETE CASCADE,
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

-- VIEWS

CREATE OR REPLACE VIEW packet_statistics AS
SELECT
    protocol,
    COUNT(*) as packet_count,
    MIN(timestamp) as first_seen,
    MAX(timestamp) as last_seen,
    SUM(packet_length) as total_bytes
FROM packets
GROUP BY protocol;

CREATE OR REPLACE VIEW daily_packet_counts AS
SELECT
    DATE(timestamp) as date,
    protocol,
    COUNT(*) as packet_count,
    SUM(packet_length) as total_bytes
FROM packets
GROUP BY DATE(timestamp), protocol
ORDER BY date DESC;

CREATE OR REPLACE VIEW top_talkers AS
SELECT
    ip.source_ip,
    ip.destination_ip,
    p.protocol,
    COUNT(*) as packet_count,
    SUM(p.packet_length) as total_bytes,
    MIN(p.timestamp) as first_seen,
    MAX(p.timestamp) as last_seen
FROM packets p
JOIN ip_packets ip ON p.packet_id = ip.packet_id
GROUP BY ip.source_ip, ip.destination_ip, p.protocol
ORDER BY packet_count DESC
LIMIT 100;

CREATE OR REPLACE VIEW top_source_ips_24h AS
SELECT
    ip.source_ip,
    COUNT(*) AS packet_count,
    SUM(p.packet_length) AS total_bytes,
    MIN(p.timestamp) AS first_packet,
    MAX(p.timestamp) AS last_packet,
    AVG(p.packet_length) AS avg_packet_size
FROM packets p
JOIN ip_packets ip ON p.packet_id = ip.packet_id
WHERE p.timestamp >= NOW() - INTERVAL '24 hours'
GROUP BY ip.source_ip
ORDER BY packet_count DESC
LIMIT 10;

CREATE OR REPLACE VIEW port_activity AS
WITH tcp_ports AS (
    SELECT
        tp.destination_port AS port,
        'TCP' AS protocol,
        COUNT(*) AS connection_attempts,
        SUM(p.packet_length) AS total_bytes
    FROM tcp_packets tp
    JOIN packets p ON tp.packet_id = p.packet_id
    GROUP BY tp.destination_port
),
udp_ports AS (
    SELECT
        up.destination_port AS port,
        'UDP' AS protocol,
        COUNT(*) AS connection_attempts,
        SUM(p.packet_length) AS total_bytes
    FROM udp_packets up
    JOIN packets p ON up.packet_id = p.packet_id
    GROUP BY up.destination_port
)
SELECT * FROM tcp_ports
UNION ALL
SELECT * FROM udp_ports
ORDER BY total_bytes DESC
LIMIT 20;

-- INDEXES

CREATE INDEX IF NOT EXISTS idx_packets_timestamp ON packets(timestamp);
CREATE INDEX IF NOT EXISTS idx_packets_protocol ON packets(protocol);
CREATE INDEX IF NOT EXISTS idx_packets_length ON packets(packet_length);
CREATE INDEX IF NOT EXISTS idx_packets_capture_interface ON packets(capture_interface);
CREATE INDEX IF NOT EXISTS idx_packets_vlan_id ON packets(vlan_id);

CREATE INDEX IF NOT EXISTS idx_ip_packets_source_ip ON ip_packets(source_ip);
CREATE INDEX IF NOT EXISTS idx_ip_packets_destination_ip ON ip_packets(destination_ip);
CREATE INDEX IF NOT EXISTS idx_ip_packets_packet_id ON ip_packets(packet_id);

CREATE INDEX IF NOT EXISTS idx_tcp_packets_source_port ON tcp_packets(source_port);
CREATE INDEX IF NOT EXISTS idx_tcp_packets_destination_port ON tcp_packets(destination_port);
CREATE INDEX IF NOT EXISTS idx_udp_packets_source_port ON udp_packets(source_port);
CREATE INDEX IF NOT EXISTS idx_udp_packets_destination_port ON udp_packets(destination_port);

-- FUNCTIONS

CREATE OR REPLACE FUNCTION cleanup_old_packets(days_old INTEGER DEFAULT 30)
RETURNS INTEGER AS $$
DECLARE
    rows_deleted INTEGER;
BEGIN
    DELETE FROM packets
    WHERE timestamp < NOW() - INTERVAL '1 day' * days_old;
    GET DIAGNOSTICS rows_deleted = ROW_COUNT;
    RETURN rows_deleted;
END; $$ LANGUAGE plpgsql;

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
END; $$ LANGUAGE plpgsql;

-- TRIGGERS

CREATE OR REPLACE FUNCTION update_timestamp()
RETURNS TRIGGER AS $$
BEGIN
    NEW.timestamp = NOW();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trigger_update_timestamp
    BEFORE UPDATE ON packets
    FOR EACH ROW
    EXECUTE FUNCTION update_timestamp();

-- FUNCTION
CREATE OR REPLACE FUNCTION insert_simple_packet(
    proto VARCHAR(10),
    length INTEGER,
    payload BYTEA DEFAULT NULL,
    interface VARCHAR(50) DEFAULT NULL,
    vlan_id_val SMALLINT DEFAULT NULL
)
RETURNS BIGINT AS $$
DECLARE
    packet_id BIGINT;
BEGIN
    INSERT INTO packets (
        protocol, packet_length, payload, capture_interface, vlan_id
    ) VALUES (
        proto, length, payload, interface, vlan_id_val
    ) RETURNING packet_id INTO packet_id;

    RETURN packet_id;
END; $$ LANGUAGE plpgsql;

-- PROCEDURES

CREATE OR REPLACE PROCEDURE bulk_insert_packets(
    packet_data JSONB[]
)
AS $$
DECLARE
    pkt JSONB;
BEGIN
    FOREACH pkt IN ARRAY packet_data
    LOOP
        INSERT INTO packets (
            protocol, packet_length, payload, capture_interface, vlan_id
        ) VALUES (
            pkt->>'protocol',
            (pkt->>'packet_length')::INTEGER,
            (pkt->>'payload')::BYTEA,
            (pkt->>'capture_interface'),
            (pkt->>'vlan_id')::SMALLINT
        );
    END LOOP;
END; $$ LANGUAGE plpgsql;

CREATE OR REPLACE PROCEDURE create_protocol_entries(
    packet_id BIGINT,
    protocol_data JSONB
)
AS $$
DECLARE
    protocol JSONB;
BEGIN
    protocol := protocol_data->>'IP';
    IF protocol IS NOT NULL THEN
        INSERT INTO ip_packets (
            packet_id, version, header_length, type_of_service,
            source_ip, destination_ip, total_length, identification,
            flags, fragment_offset, ttl, checksum
        ) VALUES (
            packet_id,
            COALESCE((protocol->>'version')::SMALLINT, 4),
            COALESCE((protocol->>'header_length')::INTEGER, 20),
            (protocol->>'type_of_service')::SMALLINT,
            '0.0.0.0'::INET + (protocol->>'source_ip')::BIGINT,
            '0.0.0.0'::INET + (protocol->>'destination_ip')::BIGINT,
            (protocol->>'total_length')::INTEGER,
            (protocol->>'identification')::INTEGER,
            (protocol->>'flags')::SMALLINT,
            (protocol->>'fragment_offset')::SMALLINT,
            COALESCE((protocol->>'ttl')::INTEGER, 64),
            (protocol->>'checksum')::INTEGER
        );
    END IF;

    protocol := protocol_data->>'TCP';
    IF protocol IS NOT NULL THEN
        INSERT INTO tcp_packets (
            packet_id, sequence_number, acknowledgment_number, source_port, destination_port, header_length, 
            fin_flag, syn_flag, rst_flag, psh_flag, ack_flag, urg_flag, ece_flag, cwr_flag,
            window_size, checksum, urgent_pointer
            ) VALUES (
            packet_id,
            (protocol->>'sequence_number')::BIGINT,
            (protocol->>'acknowledgment_number')::BIGINT,
            (protocol->>'source_port')::INTEGER,
            (protocol->>'destination_port')::INTEGER,
            (protocol->>'header_length')::INTEGER,
            (protocol->>'fin_flag')::SMALLINT,
            (protocol->>'syn_flag')::SMALLINT,
            (protocol->>'rst_flag')::SMALLINT,
            (protocol->>'psh_flag')::SMALLINT,
            (protocol->>'ack_flag')::SMALLINT,
            (protocol->>'urg_flag')::SMALLINT,
            (protocol->>'ece_flag')::SMALLINT,
            (protocol->>'cwr_flag')::SMALLINT,
            (protocol->>'window_size')::INTEGER,
            (protocol->>'checksum')::INTEGER,
            (protocol->>'urgent_pointer')::SMALLINT
            );
    END IF; 

    protocol := protocol_data->>'UDP';
    IF protocol IS NOT NULL THEN
        INSERT INTO udp_packets (
            packet_id, length, checksum
        ) VALUES (
            packet_id,
            (protocol->>'length')::INTEGER,
            (protocol->>'source_port')::INTEGER,
            (protocol->>'destination_port')::INTEGER,
            (protocol->>'checksum')::INTEGER
        );
    END IF;

    protocol := protocol->>'ICMP';
    IF protocol IS NOT NULL THEN
        INSERT INTO icmp_packets (
            packet_id, type, code, checksum, icmp_data
        ) VALUES (
            packet_id,
            (protocol->>'type')::SMALLINT,
            (protocol->>'code')::SMALLINT,
            (protocol->>'checksum')::INTEGER,
            (protocol->>'icmp_data')::BYTEA
        );
    END IF;

    protocol := protocol->>'ARP';
    IF protocol IS NOT NULL THEN
        INSERT INTO arp_packets (
            packet_id, hardware_type, protocol_type, hardware_length,
            protocol_length, operation, sender_hardware_address,
            sender_protocol_address, target_hardware_address,
            target_protocol_address
        ) VALUES (
            packet_id,
            (protocol->>'hardware_type')::SMALLINT,
            (protocol->>'protocol_type')::SMALLINT,
            (protocol->>'hardware_length')::SMALLINT,
            (protocol->>'protocol_length')::SMALLINT,
            (protocol->>'operation')::SMALLINT,
            (protocol->>'sender_hardware_address')::MACADDR,
            '0.0.0.0'::INET + (protocol->>'sender_protocol_address')::BIGINT,
            (protocol->>'target_hardware_address')::MACADDR,
            '0.0.0.0'::INET + (protocol->>'target_protocol_address')::BIGINT
        );
    END IF;
END; $$ LANGUAGE plpgsql;

CREATE OR REPLACE PROCEDURE insert_packet_with_protocol_data(
    packet_data JSONB,
    protocol_data JSONB DEFAULT NULL
)
AS $$
DECLARE
    packet_id BIGINT;
BEGIN
    SELECT insert_simple_packet(
        packet_data->>'protocol',
        (packet_data->>'packet_length')::INTEGER,
        (packet_data->>'payload')::BYTEA,
        (packet_data->>'capture_interface'),
        (packet_data->>'vlan_id')::SMALLINT
    ) INTO packet_id;

    IF protocol_data IS NOT NULL THEN
        CALL create_protocol_entries(packet_id, protocol_data);
    END IF;
END; $$ LANGUAGE plpgsql;

CREATE OR REPLACE PROCEDURE bulk_insert_packets_with_protocol_data(
    packet_data_list JSONB[]
)
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
END; $$ LANGUAGE plpgsql
