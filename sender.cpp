#include <unistd.h>
#include <pthread.h>
#include <iostream>

#include "sender.h"
#include "scheduler.h"
#include "helper-functions.h"

typedef void * (*THREADFUNCPTR)(void *);

void *PacketSender::sendLoop() {
    // isSending must be modified elsewhere to stop the loop
    while (isSending) {
        long timeToSend = this->queue->nextPacketTime();
        long now = getTimeMicros();
        if (timeToSend == -1) {
            // Back off for a short amount of time when queue is empty
            usleep(BACK_OFF_TIME_US);
            continue;
        } else if (timeToSend <= now) {
            std::cout << "Sending packet. Time to send: " << timeToSend << ", now: " << now << std::endl;
            this->queue->lockQueue();
                const pcpp::RawPacket *packetToSend = this->queue->nextPacket();
                this->sendDev->sendPacket(*packetToSend);
                this->queue->deleteHead();
            this->queue->unlockQueue();
        } 
    }
    std::cout << "PacketSender Thread terminated" << std::endl;
    pthread_exit(nullptr);
}

PacketSender::PacketSender(PacketQueue *queue, pcpp::PcapLiveDevice *sendDev)
    : queue(queue), isSending(false), sendDev(sendDev)
{}

bool PacketSender::startSending() {
    if (this->isSending) {
        return false;
    }

    pthread_attr_t attributes;
    pthread_attr_init(&attributes);

    int error = pthread_create(&this->thread, &attributes, (THREADFUNCPTR)&PacketSender::sendLoop, this);
    if (error) {
        std::cerr << "pthread creation failed: " << strerror(error) << std::endl;
        return false;
    }

    this->isSending = true;
    return true;
}

void PacketSender::stopSending() {
    if (!this->isSending) {
        return;
    }

    this->isSending = false;
    pthread_join(this->thread, nullptr);
}