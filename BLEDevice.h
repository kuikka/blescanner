/*
 * BLEDevice.h
 *
 *  Created on: Mar 26, 2014
 *      Author: kuikka
 */

#ifndef BLEDEVICE_H_
#define BLEDEVICE_H_

#include "BLEAddress.h"
#include "HciDev.h"
#include "BLEGATT.h"

namespace BLE {

class BLEDevice
{
public:
	BLEDevice();
	BLEDevice(HciDev *hciDev, const BLEAddress& bdaddr);
	virtual ~BLEDevice();
	BLEAddress& getAddress();
	HciDev* getHciDev() { return mHciDev; };
	bool connect();
	bool disconnect();

	bool onConnection(uint8_t status, uint16_t handle);
	bool onDisonnection(uint8_t reason);
	uint16_t getConnectionHandle();

protected:
	HciDev *mHciDev;
	BLEAddress mAddress;
	bool mConnected;
	uint16_t mHandle;
	BLEGATT mGatt;
};

} /* namespace BLE */

#endif /* BLEDEVICE_H_ */
