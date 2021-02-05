
#include <signal.h>
#include <unistd.h>
#include <chrono>

#include "helper-functions.h"

void setCtrlCHandler(void (*handlerFunc)(int)) {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handlerFunc;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);
}

void msleep(int milliSeconds) {
    usleep(milliSeconds * 1000);
}

long getTimeMillis() {
    auto now = std::chrono::system_clock::now();
    auto nowMs = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto epoch = nowMs.time_since_epoch();
    long duration = epoch.count();
    return duration;
}

long getTimeMicros() {
    auto now = std::chrono::system_clock::now();
    auto nowUs = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    auto epoch = nowUs.time_since_epoch();
    long duration = epoch.count();
    return duration;
}

long getTimeNanos() {
    auto now = std::chrono::system_clock::now();
    auto nowNs = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
    auto epoch = nowNs.time_since_epoch();
    long duration = epoch.count();
    return duration;
}

// #include <iostream>

// int main() {
//     std::cout << "Current time millis " << getTimeMillis() << std::endl;
//     std::cout << "Current time micros " << getTimeMicros() << std::endl;
//     std::cout << "Current time nanos  " << getTimeNanos() << std::endl;

//     return 0;
// }