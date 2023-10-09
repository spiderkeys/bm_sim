#pragma once

#include "common.hpp"

#include "message.hpp"

class Node;

struct Link {
    Node* node;
    int port;
};

class Subnet {
public:
    explicit Subnet( std::string name );

    void add_link( std::string node_id, Node* node, int port );
    void remove_link( std::string node_id );
    void send_message( const Message& msg );
    void update();

    std::string name(){ return _name; }

    const std::unordered_map<std::string, Link>& links(){ return _links; }

private:
    std::unordered_map<std::string, Link> _links;
    std::string _name;
    MessageQueue _msg_queue;
};