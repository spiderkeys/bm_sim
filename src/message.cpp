#include "message.hpp"

#include <spdlog/spdlog.h>

MessageQueue::MessageQueue( size_t max_size )
    : _max_size{ max_size }
{
}

bool MessageQueue::push( const Message& message )
{
    if( _queue.size() < _max_size ) {
        _queue.push( message );
        return true;
    }

    return false;
}

bool MessageQueue::pop( Message& message )
{
    if( _queue.size() > 0 ){
        Message temp = _queue.front();
        message = temp;
        _queue.pop();
        return true;
    }

    return false;
}

size_t MessageQueue::available() {
    return _queue.size();
}