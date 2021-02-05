#include <unistd.h>
#include <Packet.h>
#include <PcapLiveDevice.h>
#include <iostream>

#include "packet-scheduler.h"
#include "helper-functions.h"

Block::Block() : contents(nullptr), nextBlock(nullptr) {
    this->nextBlockMtxLock.lock();
}

Block::Block(pcpp::RawPacket *contents, long timeToSend)
    : contents(contents), nextBlock(nullptr), timeToSend(timeToSend)
{
    // Don't allow the next block to be accessed until it
    // has been assigned
    this->nextBlockMtxLock.lock();
}

void Block::setNext(Block *block) {
    this->nextBlock = block;
    this->nextBlockMtxLock.unlock();
}

Block *Block::getNext() {
    this->nextBlockMtxLock.lock();
    return this->nextBlock;
}

pcpp::RawPacket *Block::getContents() const {
    return this->contents;
}

long Block::getTimeToSend() const {
    return this->timeToSend;
}

ThreadSafeQueue::ThreadSafeQueue() {
    this->startBlock = new Block();
    this->endBlock = startBlock;
}

void ThreadSafeQueue::addToQueue(pcpp::RawPacket *rawPacket, long timeToSend) {
    Block *block = new Block(rawPacket, timeToSend);
    Block *previousStartBlock = this->startBlock;
    this->startBlock = block;
    // setNext will free the mutex lock, allowing the send-thread to move along
    previousStartBlock->setNext(block);
}

void ThreadSafeQueue::startSendLoop(pcpp::PcapLiveDevice *dev) {

    Block *nextBlock;
    // The first block will be empty. Acquire the next block, and whilst
    // it doesn't provide a nullptr, send the packet
    while ( (nextBlock = this->endBlock->getNext()) ) {
        
        delete this->endBlock;
        this->endBlock = nextBlock;

        // Try to send the current packet
        pcpp::RawPacket *contents = this->endBlock->getContents();
        if (contents) {
            long now = getTimeMicros();
            long timeToWeight = this->endBlock->getTimeToSend() - now;
            if (timeToWeight > 0) {
                usleep(timeToWeight);
            }
            std::cout << "Forwarding packet" << std::endl;
            dev->sendPacket(*contents);
        }
    }

    std::cout << "Sender has finished" << std::endl;
}

void ThreadSafeQueue::close() {
    this->addToQueue(nullptr, 0);
}