/**
 * Implements the methods for sending packets from the sender queue
 */

#pragma once

#include <PcapLiveDevice.h>
#include <pthread.h>

#include "scheduler.h"

#define BACK_OFF_TIME_US 500 // The time in microseconds to back off when queue is empty

class PacketSender {
private:
    PacketQueue *queue;
    volatile bool isSending;
    pcpp::PcapLiveDevice *sendDev;

    pthread_t thread;
    /**
     * The function which is passed to a thread to be executed until isSending is set to false
     */
    void *sendLoop();
public:
    PacketSender(PacketQueue *queue, pcpp::PcapLiveDevice *sendDev);
    /**
     * Creates a thread which sends data. Will stop when stopSending is called
     * \return whether creation of the sending thread was successful
     */
    bool startSending();
    /**
     * Stops the packetSender thread and cleans up.
     */
    void stopSending();
};