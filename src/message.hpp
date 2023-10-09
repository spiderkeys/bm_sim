#pragma once

#include "common.hpp"

enum class EDestAddr {
    GLOBAL = 0,
    LINK_LOCAL,
    RESOURCE_ROUTED
};

enum class EMessageType {
    UNKNOWN = 0,
    HEARTBEAT,
    RESOURCE_REQUEST,
    RESOURCE_REPLY,
    MIDDLEWARE_PUB
};

static inline std::string dest_to_string( EDestAddr dest ) {
    switch( dest ) {
        case EDestAddr::GLOBAL:             { return "G"; }
        case EDestAddr::LINK_LOCAL:         { return "LL"; }
        case EDestAddr::RESOURCE_ROUTED:    { return "RR"; }
        default:                             { return "UNKNOWN"; }
    }
}

static inline std::string msg_type_to_string( EMessageType type ) {
    switch( type ) {
        case EMessageType::HEARTBEAT:           { return "HEARTBEAT"; }
        case EMessageType::RESOURCE_REQUEST:    { return "RES_REQUEST"; }
        case EMessageType::RESOURCE_REPLY:      { return "RES_REPLY"; }
        case EMessageType::MIDDLEWARE_PUB:      { return "MIDDLEWARE_PUB"; }
        default:                                { return "UNKNOWN"; }
    }
}

// BCMP Messages
class Heartbeat {
public:
    Heartbeat(): egress_port{0}, seq{0}, liveliness_lease_dur_ms{-1} {}
    int egress_port;
    uint64_t seq;
    int32_t liveliness_lease_dur_ms;
};

class ResourceRequest {
public:
    ResourceRequest(): target{""} {}
    NodeId target;
};

class ResourceReply {
public:
    ResourceReply(): source{""} {}
    NodeId source;
};

// Middleware Messages
class MiddlewarePubSample {
public:
    // If transmitted on the GLOBAL address, topic and type will be populated
    // If transmitted on the RR address, they will be filled in by the Resource Routing logic & info in the Resource Table
    std::string topic{ "" };
    std::string type{ "" };

    std::string payload{ "" };
};

class Message {
public:
    Message() 
        : src_node{ "" }
        , dst{}
        , ingress_port{}
        , type{ EMessageType::UNKNOWN }
        , heartbeat{}
        , resource_request{}
        , resource_reply{}
        {}

    std::string src_node;
    EDestAddr dst;
    uint8_t ingress_port;

    EMessageType type;
    
    // Payloads
    Heartbeat heartbeat;
    ResourceRequest resource_request;
    ResourceReply resource_reply;
};

class MessageQueue {
public:
    explicit MessageQueue( size_t max_size );

    bool push( const Message& message );
    bool pop( Message& message );
    size_t available();

private:
    const size_t _max_size;
    std::queue<Message> _queue;
};