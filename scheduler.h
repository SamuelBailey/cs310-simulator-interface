#pragma once

#include <mutex>
#include <Packet.h>

class PacketItem {
private:
    pcpp::RawPacket rawPacket;
    long timeToSendUs;


public:
    PacketItem *next; // Towards the head
    PacketItem *previous; // Towards the tail

    /**
     * Creates a copy of the raw packet pointed to by the pointer, and stores
     * this alongside the timeToSend
     * \param rawPacket pointer to the packet to create a copy of
     * \param timeToSendUs time since the epoch in microseconds to send the packet
     */
    PacketItem(const pcpp::RawPacket *rawPacket, long timeToSendUs);

    /**
     * \return timeToSend in microseconds to send the packet since the epoch
     */
    long getTimeToSend() const;
    /**
     * \return packet contained within by const reference
     */
    const pcpp::RawPacket &getPacket() const;
};

class PacketQueue {
private:
    /**
     * Mutex lock for preventing simultaneous access to the queue
     */
    std::mutex qMtx;
    /**
     *
     */
    PacketItem *headItem;
    PacketItem *tailItem;
public:

    /**
     * Instantiate an empty queue
     */
    PacketQueue();

    /**
     * Adds a packet to the queue. Takes ownership of the packet and cleans it
     * up afterwards.
     */
    void addPacket(pcpp::RawPacket *rawPacket, long timeToSendUs);
    /**
     * \return The timeToSendUs field for the packet at the head of the queue
     * or -1 if the head contains nullptr
     */
    long nextPacketTime();
    /**
     * Obtain the mutex lock for the queue
     */
    void lockQueue();
    /**
     * Free the mutex lock for the queue
     */
    void unlockQueue();
    /**
     * Surround calls to this function with lockQueue and unlockQueue to enusre the
     * data doesn't disappear whilst it is in use
     * \return a const reference to RawPacket at the head of the list. nullptr
     * if the list is empty
     */
    const pcpp::RawPacket &nextPacket();
    /**
     * Frees the memory for the packet currently at the head of the queue, and
     * advances the queue
     * 
     * Make sure this function is surrounded with calls to lockQueue and unlockQueue,
     * and make sure that you know which packet you are removing, given the threading
     * 
     * WARNING: This method could delete data that is in-use. Do not use until
     * you are sure that the head element is no longer in-use. Be careful if
     * using threads.
     */
    void deleteHead();
    /**
     * Cleanup the packet queue
     */
    void close();
};

class PacketScheduler {
private:

public:
    /**
     * \param timeToSendUs The time to send the packet since the epoch
     */
    void schedulePacket(long timeToSendUs);
};