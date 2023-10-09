#include "subnet.hpp"

#include "node.hpp"

#include <spdlog/spdlog.h>

Subnet::Subnet( std::string name )
    : _name{ name }
    , _msg_queue{ 1000 }
{

}

void Subnet::add_link( std::string node_id, Node* node, int port ){
    _links[ node_id] = { node, port };
}

void Subnet::remove_link( std::string node_id ){
    _links.erase( node_id );
}

void Subnet::send_message( const Message& msg ) {
    _msg_queue.push( msg );
}

void Subnet::update() {
    

    // For each queued message, deliver the message to the RX queue of each link other than the original sender
    auto count = _msg_queue.available();
    while( count > 0 ) {
        Message msg{};
        if( _msg_queue.pop( msg ) ) {

            // Look up port for sending node
            const auto& link = _links.at( msg.src_node );
            uint8_t egress_port = link.port;

            // TODO: Send to nodes
            spdlog::debug( "[{}] Delivering msg[{}] from SRC[{}:{}] to DST[{}]", _name, msg_type_to_string( msg.type ), msg.src_node, (int)egress_port, dest_to_string( msg.dst ) );
            
            for( auto& dest_link: _links ) {
                if( dest_link.first == msg.src_node ) {
                    spdlog::debug( "Skipping self ({})", msg.src_node );
                    continue;
                }

                spdlog::debug( "Receiving to: {}:{}", dest_link.second.node->id(), dest_link.second.port );
                dest_link.second.node->receive( msg, dest_link.second.port );
            }
        }

        count--;
    }
}