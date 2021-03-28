#include <iostream>

#include "tsupport.hh"

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    TestArgs t(argc, argv);
    auto f = t.create_fortuna();
    
    // run & stop
    title("Run & stop");
    t.run_code(R"(
        nop       ; 0
        nop       ; 1
    end: jp end   ; 2
    )");
    f->run();
    std::this_thread::sleep_for(10ms);
    assert_eq("Check that last event is no event", EventType::NoEvents, f->last_event());
    f->stop();
    std::this_thread::sleep_for(1ms);
    assert_eq("Check that last event is stop", EventType::Stopped, f->last_event());
    assert_eq("Check that PC == 2", 2, f->z80_info().pc);
    
    // TODO - run until breakpoint
    
    // TODO - run until print char
    
    // TODO - run and send keypress
    
    // TODO - next
    
    // TODO - try to change memory while running
    
    std::cout << "Done.\n";
}
