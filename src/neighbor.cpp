#include "neighbor.hpp"

void NeighborTable::insert( NeighborEntry entry )
{
    _neighbors[ entry.node_id ] = entry;
}

bool NeighborTable::find( NodeId id, NeighborEntry& entry )
{
    auto count = _neighbors.count( id );
    if( count )
    {
        entry = _neighbors.at( id );
        return true;
    }
    return false;
}