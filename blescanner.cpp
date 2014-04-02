#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "HciDev.h"
#include "BLEDevice.h"
#include "BLEAddress.h"
#include "BLEScanListener.h"
#include "MainLoop.h"

typedef std::list <BLE::HciDev*> HciDevList;
HciDevList devices;


class ScanListener : public BLE::BLEScanListener {
	virtual void onAdvertisingReport(BLE::HciDev *hciDev,
			EventType type,
			BLE::BLEAddress from,
			int8_t rssi,
			const uint8_t *data,
			size_t datalen)
	{
		static int foo = 0;

		std::cout << "Got advertisement data\n";
		printf("  type=%d\n", type);
		printf("  from=%02x:%02x:%02x:%02x:%02x:%02x\n",
				from.address.b[0],
				from.address.b[1],
				from.address.b[2],
				from.address.b[3],
				from.address.b[4],
				from.address.b[5]);
		printf("  rssi=%d\n", rssi);

		if (foo++ == 0) {
			printf("Connecting!");
			BLE::BLEDevice dev(hciDev, from);

			BLE::HciDev * hci = devices.front();
			hci->leConnect(&dev);

		}
	}
};

int main(int argc, char *argv[])
{
	ScanListener scanListener;
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
		}
	}

	if (random)
		addr.type = BLE::BLEAddress::RANDOM;

	if (hciDev == -1) {
		while (1) {
			BLE::HciDev *dev = new BLE::HciDev(i++, &mainloop);
			if (dev->open()) {
				dev->devInfo();
				devices.push_back(dev);
			} else {
				delete dev;
				break;
			}
		}
	} else {
		BLE::HciDev *dev = new BLE::HciDev(0, &mainloop);
		if (dev->open()) {
			dev->devInfo();
			devices.push_back(dev);
		} else {
			delete dev;
			return 1;
		}
	}

	for (HciDevList::iterator i = devices.begin();
			i != devices.end();
			++i)
	{
		BLE::HciDev *dev = *i;
		dev->down();
		dev->up();

		if (scan) {
			dev->leSetScanParameters(BLE::HciDev::ACTIVE, 0x12, 0x12, BLE::BLEAddress::PUBLIC, BLE::HciDev::ALL);
			dev->setScanListener(&scanListener);
			dev->leScanEnable(true, true);
		} else {
			dev->leScanEnable(false, true);
		}
	}

	if (connect) {
		BLE::HciDev *dev = devices.front();
		BLE::BLEDevice ble(dev, addr);
		ble.connect();
	}


	mainloop.run();
	std::cout << "Mainloop exited\n";
	return 0;
}
