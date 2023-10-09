#include "resource.hpp"

bool ResourceTable::exists( std::string key )
{
    return ( _resources.count( key ) > 0 );
}

int32_t ResourceTable::find_id( std::string key )
{
    auto count = _resources.count( key );
    if( count )
    {
        return _resources.at( key );
    }
    return -1;
}


std::string ResourceTable::find_key( uint32_t id )
{
    for( const auto& res : _resources )
    {
        if( res.second == id )
        {
            return res.first;
        }
    }
    return {};
}

bool ResourceTable::create( std::string key )
{
    if( !exists( key ) )
    {
        _resources[ key ] = ++_last_assigned_id;
        return true;
    }
    return false;
}

bool ResourceTable::erase( std::string key )
{
    if( exists( key ) )
    {
        _resources.erase( key );
        return true;
    }
    return false;
}