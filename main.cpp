
#include <iostream>
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
#include <PlatformSpecificUtils.h>

#include "interrupt-handler.h"

static pcpp::PcapLiveDevice *dev;

static void cleanUp() {
    std::cout << std::endl;
    std::cout << "Stopping capture of packets" << std::endl;
    dev->stopCapture();
    std::cout << "Closing dev" << std::endl;
    dev->close();
    std::cout << "Finished cleanup, exiting program" << std::endl;
    exit(0);
}

static void ctrlCHandler(int) {
    cleanUp();
}

static void onPacketArrival(pcpp::RawPacket *rawPacket, pcpp::PcapLiveDevice *dev, void *cookie) {
    std::string *progName = (std::string *)cookie;
    // std::cout << "Received a packet within program " << *progName << std::endl;
    pcpp::Packet parsedPacket(rawPacket);
    if (parsedPacket.isPacketOfType(pcpp::ICMP)) {
        std::cout << "Received a ping" << std::endl;
    }
}

int main() {
    std::string programName = "Geoff";

    setCtrlCHandler(ctrlCHandler);

    std::cout << "Hello, world" << std::endl;

    // dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp("vnic0");
    dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByName("wlp5s0");

    if (!dev) {
        std::cerr << "Unable to acquire device" << std::endl;
        exit(1);
    } else {
        std::cout << "Acquired device" << std::endl;
    }

    if (!dev->open()) {
        std::cerr << "Unable to open the device" << std::endl;
        exit(1);
    }

    dev->startCapture(onPacketArrival, &programName);
    PCAP_SLEEP(100);

    dev->stopCapture();

    dev->close();

    return 0;
}