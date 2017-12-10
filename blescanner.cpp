#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <set>

#include "config.h"
#include "HciDev.h"
#include "BLEDevice.h"
#include "BLEAddress.h"
#include "BLEScanListener.h"
#include "MainLoop.h"

typedef std::list <BLE::HciDev*> HciDevList;
HciDevList hciDevices;


bool connect_all = false;

class ScanListener : public BLE::BLEScanListener, public std::enable_shared_from_this<ScanListener>  {
	virtual void onAdvertisingReport(BLE::HciDev &hciDev,
			EventType type,
			BLE::BLEAddress from,
			int8_t rssi,
			const uint8_t *data,
			size_t datalen)
	{
#if 1
		std::cout << "Got advertisement data ";
		std::cout << from;
		std::cout << " RSSI=" << (int) rssi << "\n";
#endif
#if 0
		BLE::BLEDevice *dev = hciDev->findDeviceByAddress(from);
		if (dev == NULL) {
			std::cout << "Connecting to: " << from << "\n";
			BLE::BLEDevice *dev = new BLE::BLEDevice(hciDev, from);
			dev->connect();
		} else {
			if (dev->getState() == BLE::BLEDevice::DISCONNECTED) {
				std::cout << "Reconnecting to: " << dev->getAddress() << "\n";
				dev->connect();
			}
		}
#endif
	}
};

int main(int argc, char *argv[])
{
	shared_ptr<ScanListener> scanListener = std::make_shared<ScanListener>();
	MainLoop mainloop;
	BLE::BLEAddress addr;
	int opt;

	unsigned i = 0;

	int hciDev = -1;
	bool scan = false;
	bool connect = false;
	bool random = false;

	while((opt = getopt(argc, argv, "d:c:rps")) != -1) {
		switch (opt) {
		case 'd':
			hciDev = atoi(optarg);
			break;
		case 'c':
			connect = true;
			addr = BLE::BLEAddress(optarg, BLE::BLEAddress::PUBLIC);
			break;
		case 'r':
			random = true;
			break;
		case 'p':
			random = false;
			break;
		case 's':
			scan = true;
			break;
		case 'a':
			connect_all = true;
		}
	}

	if (!scan && !connect) {
		printf("Whatchawanttodo?\n");
		return 1;
	}

	if (random)
		addr.type = BLE::BLEAddress::RANDOM;

	if (hciDev == -1) {
		while (1) {
			BLE::HciDev *dev = new BLE::HciDev(i++, &mainloop);
			if (dev->open()) {
				dev->devInfo();
				hciDevices.push_back(dev);
			} else {
				delete dev;
				break;
			}
		}
	} else {
		BLE::HciDev *dev = new BLE::HciDev(0, &mainloop);
		if (dev->open()) {
			dev->devInfo();
			hciDevices.push_back(dev);
		} else {
			delete dev;
			return 1;
		}
	}

	for (HciDevList::iterator i = hciDevices.begin();
			i != hciDevices.end();
			++i)
	{
		BLE::HciDev *dev = *i;
//		dev->down();
//		dev->up();
		dev->leScanEnable(false, true);
		dev->leCancelConnection();
		dev->leClearWhiteList();

		if (scan) {
			dev->leSetScanParameters(BLE::HciDev::ACTIVE, 0x12, 0x12, BLE::BLEAddress::PUBLIC, BLE::HciDev::ALL);
			dev->setScanListener(scanListener);
			dev->leScanEnable(true, true);
		}
	}

	if (connect) {
		BLE::HciDev *dev = hciDevices.front();
		BLE::BLEDevice ble(dev, addr);
		ble.connect();
	}


	mainloop.run();
	std::cout << "Mainloop exited\n";
	return 0;
}
