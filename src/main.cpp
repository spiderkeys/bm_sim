#include "node.hpp"
#include "subnet.hpp"

#include <functional>
#include <chrono>
#include <thread>
#include <csignal>

#include <spdlog/spdlog.h>
#include <iostream>

namespace {
    std::function<void(int)> signal_handler_func;
    void signal_handler(int signal) { signal_handler_func( signal ); }
}

int main()
{
    bool terminate = false;

    // Example Topology:
    // A:0 <SUBNET_AB> 0::B::1 <SUBNET_BC> 0::C
    Subnet subnet_ab{ "ab" };
    Subnet subnet_bc{ "bc" };
    std::vector<Subnet*> subnets = { &subnet_ab, &subnet_bc };

    Node a{ "a", std::vector<Port>{ Port{ &subnet_ab, 0 } } };
    Node b{ "b", std::vector<Port>{ Port{ &subnet_ab, 0 }, Port{ &subnet_bc, 1 } } };
    Node c{ "c", std::vector<Port>{ Port{ &subnet_bc, 0 } } };
    std::vector<Node*> nodes { &a, &b, &c };

    // Register handlers for SIGINT and SIGTERM
    signal_handler_func = [&]( int signal_id )
    {
        (void)signal_id;
        terminate = true;
    };

    struct sigaction default_action;
    struct sigaction term_action;

    term_action.sa_handler = &signal_handler;
    term_action.sa_flags = SA_RESTART;
    sigfillset( &term_action.sa_mask );

    sigaction( SIGINT, &term_action, &default_action );
    sigaction( SIGTERM, &term_action, &default_action );

    spdlog::info( "Starting simulation" );

    auto next_tick = std::chrono::steady_clock::now();
    while( !terminate ) {
        next_tick += std::chrono::milliseconds( 1 );

        for( auto& subnet: subnets ) {
            subnet->update();
        }

        for( auto& node: nodes )
        {
            node->update();
        }

        std::this_thread::sleep_until( next_tick );
    }

    std::cout << std::endl;
    spdlog::info( "Exiting" );
    

    return 0;
}