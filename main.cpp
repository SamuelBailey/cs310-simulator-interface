
#include <iostream>
#include <thread>
#include <chrono>
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
// #include <PlatformSpecificUtils.h>
#include <Logger.h>

#include "helper-functions.h"
#include "scheduler.h"
#include "sender.h"

#define PACKET_DELAY_US 2000000 // Microseconds to delay packet transmission

static volatile bool running = true;

// static ThreadSafeQueue queue;
static pcpp::PcapLiveDevice *rxDev;
static pcpp::PcapLiveDevice *txDev;

// Globals for managing threads
PacketQueue packetQueue;
PacketSender packetSender(&packetQueue, txDev);

static void ctrlCHandler(int) {
    std::cout << "\nInterrupt detected, performing graceful shutdown" << std::endl;
    running = false;
}

static void schedulePacketAfter(pcpp::RawPacket *rawPacket, int milliseconds) {
    msleep(milliseconds);
    std::this_thread::sleep_for(std::chrono::milliseconds{milliseconds});
}

// void startSendingPackets(pcpp::PcapLiveDevice *dev) {
//     queue.startSendLoop(dev);
// }

static void onPacketArrival(pcpp::RawPacket *rawPacket, pcpp::PcapLiveDevice *dev, void *cookie) {
    std::string *progName = (std::string *)cookie;
    // std::cout << "Received a packet within program " << *progName << std::endl;
    pcpp::Packet parsedPacket(rawPacket);
    if (parsedPacket.isPacketOfType(pcpp::ICMP)) {
        std::cout << "Received a ping" << std::endl;
    }

    std::cout << "Received packet with pointer " << rawPacket << std::endl;

    // Check length of packet being sent
    std::cout << "Length of packet: " << rawPacket->getRawDataLen() << std::endl;

    std::cout << "Queue length before adding: " << packetQueue.queueLength() << std::endl;
    packetQueue.addPacket(rawPacket, getTimeMicros() + PACKET_DELAY_US);
    std::cout << "Queue length after adding: " << packetQueue.queueLength() << std::endl;
    // txDev->sendPacket(*rawPacket);

    // if (dev->sendPacket(*rawPacket)) {
    //     std::cout << "Forwarded packet" << std::endl;
    // } else {
    //     std::cout << "Failed to forward packet" << std::endl;
    // }
}

int main() {
    std::string programName = "Geoff";

    setCtrlCHandler(ctrlCHandler);

    // pcpp::LoggerPP::getInstance().setLogLevel(pcpp::LogModule::PcapLogModuleLiveDevice, pcpp::LoggerPP::LogLevel::Debug);

    std::cout << "Hello, world" << std::endl;

    rxDev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName("vnic0");
    txDev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName("vnic1");
    // pcpp::PcapLiveDevice *dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName("wlp5s0");

    if (!rxDev || !txDev) {
        std::cerr << "Unable to acquire device" << std::endl;
        exit(1);
    } else {
        std::cout << "Acquired device" << std::endl;
    }

    if (!rxDev->open() || !txDev->open()) {
        std::cerr << "Unable to open the device" << std::endl;
        exit(1);
    }

    std::cout << "Receive device MTU: " << rxDev->getMtu() << std::endl;
    std::cout << "Send device MTU: " << txDev->getMtu() << std::endl;

    packetSender = PacketSender(&packetQueue, txDev);
    if (!packetSender.startSending()) {
        std::cerr << "Unable to start packet sending" << std::endl;
        exit(1);
    }

    if (!rxDev->startCapture(onPacketArrival, &programName)) {
        std::cerr << "Unable to start packet capture for receiving device" << std::endl;
        exit(1);
    }

    std::cout << "Running packet capture... Press Ctrl + C to exit" << std::endl;
    // Relies on Ctrl+C to set running to false
    while (running) {
        msleep(100);
    }

    std::cout << "Stopping capture of packets" << std::endl;
    rxDev->stopCapture();
    // Needs to be after capture has been stopped, so more are not added to the
    // queue

    std::cout << "Stopping transmission" << std::endl;
    packetSender.stopSending();

    std::cout << "Cleanup packet queue" << std::endl;
    packetQueue.close();

    std::cout << "Closing dev" << std::endl;
    rxDev->close();
    txDev->close();

    std::cout << "Exiting program" << std::endl;
}