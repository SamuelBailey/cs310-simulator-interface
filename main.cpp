
#include <iostream>
#include <thread>
#include <chrono>
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
// #include <PlatformSpecificUtils.h>
#include <Logger.h>

#include "packet-scheduler.h"
#include "helper-functions.h"

#define WAIT_TIME_US 5000000

static volatile bool running = true;

static ThreadSafeQueue queue;
static pcpp::PcapLiveDevice *rxDev;
static pcpp::PcapLiveDevice *txDev;

static void ctrlCHandler(int) {
    std::cout << "\nInterrupt detected, performing graceful shutdown" << std::endl;
    running = false;
}

static void schedulePacketAfter(pcpp::RawPacket *rawPacket, int milliseconds) {
    msleep(milliseconds);
    std::this_thread::sleep_for(std::chrono::milliseconds{milliseconds});
}

void startSendingPackets(pcpp::PcapLiveDevice *dev) {
    queue.startSendLoop(dev);
}

static void onPacketArrival(pcpp::RawPacket *rawPacket, pcpp::PcapLiveDevice *dev, void *cookie) {
    std::string *progName = (std::string *)cookie;
    // std::cout << "Received a packet within program " << *progName << std::endl;
    pcpp::Packet parsedPacket(rawPacket);
    if (parsedPacket.isPacketOfType(pcpp::ICMP)) {
        std::cout << "Received a ping" << std::endl;
    }

    std::cout << "Received packet with pointer " << rawPacket << std::endl;

    // queue.addToQueue(rawPacket, getTimeMicros() + WAIT_TIME_US);

    // Check length of packet being sent
    std::cout << "Length of packet: " << rawPacket->getRawDataLen() << std::endl;

    txDev->sendPacket(*rawPacket);

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

    if (!rxDev->startCapture(onPacketArrival, &programName)) {
        std::cerr << "Unable to start packet capture for receiving device" << std::endl;
        exit(1);
    }

    std::cout << "Running packet capture... Press Ctrl + C to exit" << std::endl;
    // Relies on Ctrl+C to set running to false
    while (running) {
        msleep(5);
    }

    std::cout << "Stopping capture of packets" << std::endl;
    rxDev->stopCapture();
    // Needs to be after capture has been stopped, so more are not added to the
    // queue
    std::cout << "Closing queue" << std::endl;
    queue.close();

    std::cout << "Closing dev" << std::endl;
    rxDev->close();
    txDev->close();

    std::cout << "Exiting program" << std::endl;
}