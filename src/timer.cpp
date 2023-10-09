#include "timer.hpp"

#include <thread>

Timer::Timer( std::chrono::microseconds rate_in )
    : _rate{ rate_in }
    , _next{ std::chrono::steady_clock::now() }
    , _missed{ 0 }
{}

// Resets the waiting period
void Timer::reset()
{
    _next = std::chrono::steady_clock::now();
}

uint64_t Timer::check()
{
    auto elapsed = std::chrono::steady_clock::now() - _next;
    if( elapsed > _rate )
    {
        uint64_t elapsed_us = static_cast<uint64_t>( std::chrono::duration_cast<std::chrono::microseconds>( elapsed ).count() );

        // Calculate number of whole cycles elapsed
        uint64_t cycles = static_cast<uint64_t>( elapsed_us / _rate.count() );
        uint64_t missed = cycles - 1;

        // Update total missed cycles
        _missed += missed;

        // Align to next multiple of rate period based on number of cycles
        _next += std::chrono::microseconds( ( cycles ) * _rate.count() );

        return cycles;
    }
    else
    {
        return 0;
    }
}

uint64_t Timer::missed_cycles()
{
    return _missed;
}