#pragma once

#include <Packet.h>
#include <mutex>
#include <chrono>

/**
 * Container for holding each item of information for the ThreadSafeQueue
 */
class Block {
private:
    pcpp::RawPacket *contents;
    Block *nextBlock;
    std::mutex nextBlockMtxLock;
    // useconds since epoch
    long timeToSend;
public:
    /**
     * Create an empty block
     */
    Block();
    /**
     * Create a block containing the packet
     */
    Block(pcpp::RawPacket *contents, long timeToSend);
    /**
     * Sets the next block in the queue. Unlocks the mutex lock to allow it to be sent
     * The block should already have all parameters set for sending
     * \param block The block to add to the queue
     */
    void setNext(Block *block);
    /**
     * \return next Block in the queue. Acquires mutex lock before proceeding,
     * so performs wait if required
     */
    Block *getNext();
    /**
     * Get the contents of the Block - the pointer to the packet contained within it
     */
    pcpp::RawPacket *getContents() const;
    /**
     * Get the time in microseconds since epoch when the packet is due to be sent
     */
    long getTimeToSend() const;
};

/**
 * Linked list implementation of queue which allows a SINGLE thread to enqueue
 * and a separate SINGLE thread to dequeue
 */
class ThreadSafeQueue {
private:
    // Add to the start block
    Block *startBlock;
    // Remove from the end block
    Block *endBlock;
public:
    /**
     * Create an empty queue. The first item in the queue will be empty. This is
     * handled by the startSendLoop function.
     */
    ThreadSafeQueue();

    /**
     * Add the packet to the end of the queue
     * \param rawPacket The packet to add
     * \param timeToSend The time in microseconds since the epoch for when to send
     */
    void addToQueue(pcpp::RawPacket *rawPacket, long timeToSend);

    /**
     * Creates a thread to send the data
     */
    void startSendLoop(pcpp::PcapLiveDevice *dev);

    /**
     * Allows the sendLoop thread to finish sending the remaining blocks and
     * gracefully close, freeing up memory and threads.
     */
    void close();
};

class PacketScheduler {

};