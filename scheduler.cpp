#include <Packet.h>

#include "scheduler.h"

PacketItem::PacketItem(const pcpp::RawPacket *rawPacket, long timeToSendUs)
    : rawPacket(*rawPacket), timeToSendUs(timeToSendUs)
{}

long PacketItem::getTimeToSend() const {
    return this->timeToSendUs;
}

const pcpp::RawPacket &PacketItem::getPacket() const {
    return this->rawPacket;
}

PacketQueue::PacketQueue() : headItem(nullptr), tailItem(nullptr) {}

void PacketQueue::addPacket(pcpp::RawPacket *rawPacket, long timeToSendUs) {
    PacketItem *thisPacket = new PacketItem(rawPacket, timeToSendUs);

    this->qMtx.lock();

        PacketItem *nextPacket = this->tailItem;
        PacketItem *previousPacket = nullptr;
        while (nextPacket && nextPacket->getTimeToSend() > timeToSendUs) {
            previousPacket = nextPacket;
            nextPacket = nextPacket->next;
        }

        if (nextPacket) {
            nextPacket->previous = thisPacket;
            thisPacket->next = nextPacket;
        } else {
            this->headItem = thisPacket;
            thisPacket->next = nullptr;
        }

        if (previousPacket) {
            previousPacket->next = thisPacket;
            thisPacket->previous = previousPacket;
        } else {
            this->tailItem = thisPacket;
            thisPacket->previous = nullptr;
        }

    this->qMtx.unlock();
}

long PacketQueue::nextPacketTime() {
    long time;
    this->qMtx.lock();
        if (this->headItem) {
            time = this->headItem->getTimeToSend();
        } else {
            time = -1;
        }
    this->qMtx.unlock();
    return time;
}

void PacketQueue::lockQueue() {
    this->qMtx.lock();
}

void PacketQueue::unlockQueue() {
    this->qMtx.unlock();
}

const pcpp::RawPacket &PacketQueue::nextPacket() {
    const pcpp::RawPacket &nextPacket = this->headItem->getPacket();
    return nextPacket;
}

void PacketQueue::deleteHead() {
    PacketItem *oldHead = nullptr;
    if (this->headItem) {
        oldHead = this->headItem;
        this->headItem = this->headItem->next;
    }
    delete oldHead;
}

void PacketQueue::close() {
    this->qMtx.lock();
        PacketItem *currentPacket = this->tailItem;
        while (currentPacket) {
            PacketItem *nextPacket = currentPacket->next;
            delete currentPacket;
            currentPacket = nextPacket;
        }
    this->qMtx.unlock();
}