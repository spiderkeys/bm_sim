#pragma once

#include "neighbor.hpp"
#include "resource.hpp"
#include "timer.hpp"
#include "message.hpp"

class Subnet;

struct Port {
    Port() : subnet{nullptr}, index{ -1 } {}
    Port( Subnet* s, int i ): subnet{ s }, index{ i } {}
    Subnet* subnet;
    int index;
};

class Node {
    static constexpr std::chrono::milliseconds NEIGHBOR_LIVELINESS_CHECK_PERIOD_MS{ 2000 };
    static constexpr std::chrono::milliseconds HEARTBEAT_PERIOD_MS{ 100 };

public:
    Node( NodeId id, const std::vector<Port>& port_map  );

    NodeId id() const { return _id; }

    void update();

    void receive( const Message& msg, uint8_t port );

    void create_subscriber( std::string topic, std::string type, std::function<void(MiddlewarePubSample)> cb );
    void create_publisher( std::string topic, std::string type );

private:
    void process_messages();
    void handle_message( const Message& msg );
    void handle_heartbeat( const Message& msg );

    void update_neighbor_table();

    void send_heartbeat();

    NodeId          _id;

    std::vector<Port> _ports;

    NeighborTable   _nt;
    ResourceTable   _rt;

    Timer _heartbeat_timer;
    Timer _neighbor_lease_timer;

    uint64_t _heartbeat_seq{0};

    MessageQueue _rx_queue;
    
};