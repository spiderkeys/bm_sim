#pragma once

#include <chrono>

class Timer {
public:
    explicit Timer( std::chrono::microseconds rate_in );

    void reset();
    uint64_t check();

    uint64_t missed_cycles();

private:
    const std::chrono::microseconds         _rate;
    std::chrono::steady_clock::time_point   _next;
    uint64_t                                _missed;
};