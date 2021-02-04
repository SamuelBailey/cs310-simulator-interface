
#include <iostream>
#include <PcapLiveDevice.h>
#include <PcapLiveDeviceList.h>
#include <PlatformSpecificUtils.h>

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

    std::cout << "Hello, world" << std::endl;

    pcpp::PcapLiveDevice *dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp("192.168.0.10");

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
    PCAP_SLEEP(10);

    dev->stopCapture();

    dev->close();

    return 0;
}