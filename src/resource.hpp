#pragma once

#include "common.hpp"

enum class EInterestType {
    PUBLICATION,
    SUBSCRIPTION
};

struct Resource {
    std::string key;
    uint32_t    id;
};

struct Interest {
    NodeId          owner;
    std::string     key;
    EInterestType   type;
    std::string     metadata;
};

class ResourceTable {
public:
    bool create( std::string key );
    bool erase( std::string key );
    bool exists( std::string key );
    int32_t find_id( std::string key );
    std::string find_key( uint32_t id );

private:
    std::unordered_map<std::string, uint32_t> _resources;
    uint32_t _last_assigned_id = 0;
};