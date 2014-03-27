#include <iostream>
#include "HciDev.h"
#include "BLEScanListener.h"
#include "MainLoop.h"

class ScanListener : public BLE::BLEScanListener {
	virtual void onAdvertisingReport(EventType type,
			BLE::BLEAddress from,
			int8_t rssi,
			const uint8_t *data,
			size_t datalen)
	{
		std::cout << "Got advertisement data\n";
	}
};

int main(int argc, char *argv[])
{
	ScanListener scanListener;
	MainLoop mainloop;
	unsigned i = 0;

	std::list <BLE::HciDev*> devices;

	while (1) {
		BLE::HciDev *dev = new BLE::HciDev(i++, &mainloop);
		if (dev->open()) {
			dev->devInfo();

			dev->leSetScanParameters(BLE::HciDev::ACTIVE, 0x12, 0x12, BLE::HciDev::PUBLIC, BLE::HciDev::ALL);
			dev->leSetScanParameters(BLE::HciDev::ACTIVE, 0x14, 0x14, BLE::HciDev::PUBLIC, BLE::HciDev::ALL);
			dev->setScanListener(&scanListener);
			dev->leScanEnable(true, true);

			devices.push_back(dev);
		} else {
			delete dev;
			break;
		}
	}

	mainloop.run();
	std::cout << "Mainloop exited\n";
	return 0;
}
