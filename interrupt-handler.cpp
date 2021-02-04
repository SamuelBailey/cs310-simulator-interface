
#include <signal.h>

#include "interrupt-handler.h"

void setCtrlCHandler(void (*handlerFunc)(int)) {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handlerFunc;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);
}