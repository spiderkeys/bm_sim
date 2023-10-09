#include "node.hpp"

#include <iostream>
#include <spdlog/spdlog.h>

#include "message.hpp"
#include "subnet.hpp"


Node::Node( NodeId id, const std::vector<Port>& port_map )
    : _id{ id }
    , _ports{ port_map }
    , _heartbeat_timer{ HEARTBEAT_PERIOD_MS }
    , _neighbor_lease_timer{ NEIGHBOR_LIVELINESS_CHECK_PERIOD_MS }
    , _rx_queue{ 1000 }
{
    for( auto port: _ports ){
        spdlog::info( "Registering port {}:{} on subnet:{}", this->id(), port.index, port.subnet->name() );
        port.subnet->add_link( this->id(), this, port.index );
    }
}


void Node::update() {
    // Handle queued RX messages
    process_messages();

    if( _neighbor_lease_timer.check() ) {
        update_neighbor_table();
    }

    if( _heartbeat_timer.check() ) {
        send_heartbeat();
    }
}

void Node::receive( const Message& msg, uint8_t port )
{
    Message msg_copy = msg;
    msg_copy.ingress_port = port;

    _rx_queue.push( msg_copy );
}

void Node::create_subscriber( std::string topic, std::string type, std::function<void(MiddlewarePubSample)> cb )
{
    // Check resource table to see if local representation already exists
    int32_t rid = -1;
    if( _rt.exists( topic ) ) {
        // Get the existing local RID
        rid = _rt.find_id( topic );
    } else {
        _rt.create( topic );
        rid = _rt.find_id( topic );
    }

    // Create new subscription
    
}

void Node::create_publisher( std::string topic, std::string type )
{

}

void Node::process_messages() {
    Message msg{};
    size_t pending = _rx_queue.available();
    
    while( pending > 0 ) {
        if( _rx_queue.pop( msg ) ) {
            handle_message( msg );
        }

        pending--;
    }
}

void Node::handle_message( const Message& msg )
{
    if( msg.src_node == id() ) {
        spdlog::error( "Received msg from self: {}", id() );
        return;
    }

    // Forward global messages to other ports
    if( msg.dst == EDestAddr::GLOBAL ) {
        for( auto& port : _ports ) {
            if( port.index != msg.ingress_port ){
                spdlog::info( "Forwarding message globally" );
                port.subnet->send_message( msg );
            }
        }
    }

    if( msg.dst == EDestAddr::RESOURCE_ROUTED ){
        // TODO: Handle forwarding based on current resource table knowlegde
    }
    
    // Handle message locally, depending on contents
    switch( msg.type ) {
        case EMessageType::HEARTBEAT: { handle_heartbeat( msg ); break; }
        default: { spdlog::warn( "UNHANDLED MESSAGE" ); }
    }
}

void Node::handle_heartbeat( const Message& msg )
{
    spdlog::debug( "NODE[{}:{}] got heartbeat from NODE[{}:{}]", id(), msg.ingress_port, msg.src_node, msg.heartbeat.egress_port );

    NeighborEntry neighbor;
    bool found_neigbor = _nt.find( msg.src_node, neighbor );
    if( found_neigbor ){
        // Refresh liveliness lease
        _nt.neighbors()[msg.src_node].last_heartbeat = std::chrono::steady_clock::now();
    }
    else {
        // Create new entry
        neighbor.node_id = msg.src_node;
        neighbor.remote_egress_port = msg.heartbeat.egress_port;
        neighbor.local_ingress_port = msg.ingress_port;
        neighbor.liveliness_lease_dur_ms = msg.heartbeat.liveliness_lease_dur_ms;
        neighbor.last_heartbeat = std::chrono::steady_clock::now();

        spdlog::info( "NODE[{}] discovered NODE[{}:{}] on port[{}]", id(), msg.src_node, msg.heartbeat.egress_port, msg.ingress_port );
        _nt.insert( neighbor );

        // TODO: Trigger resource discovery tasks
    }
}

void Node::update_neighbor_table()
{
    auto now = std::chrono::steady_clock::now();
    for( auto it = _nt.neighbors().begin(); it != _nt.neighbors().end(); ) {
        auto time_since_last_hb = std::chrono::duration_cast<std::chrono::milliseconds>( now - it->second.last_heartbeat );
        if( time_since_last_hb > std::chrono::milliseconds( it->second.liveliness_lease_dur_ms ) ) {
            // Liveliness lost
            spdlog::info( "NODE[{}] detected liveliness loss of NODE[{}] ( {} > {} )", id(), it->second.node_id, time_since_last_hb.count(), it->second.liveliness_lease_dur_ms );
            it = _nt.neighbors().erase( it );
            
            // TODO: Trigger resource discovery tasks
        }
        else {
            it++;
        }
    }
}

void Node::send_heartbeat() {
    for( auto& port : _ports ) {
        spdlog::debug( "Sending from port: {}:{} on subnet {}", id(), port.index, port.subnet->name() );
        Message msg{};
        msg.src_node = id();
        msg.dst = EDestAddr::LINK_LOCAL;
        msg.type = EMessageType::HEARTBEAT;

        msg.heartbeat.egress_port = port.index;
        msg.heartbeat.seq = _heartbeat_seq++;
        msg.heartbeat.liveliness_lease_dur_ms = 3000;

        port.subnet->send_message( msg );
    }
}