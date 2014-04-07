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
	typedef enum {
		DISCONNECTED,
		CONNECTING,
		CONNECTED,
		DISCONNECTING,
	} DeviceState;
public:
	BLEDevice();
	BLEDevice(HciDev *hciDev, const BLEAddress& bdaddr);
	virtual ~BLEDevice();
	BLEAddress& getAddress();
	HciDev* getHciDev() { return mHciDev; };
	bool connect();
	bool disconnect();

	bool onConnection(uint8_t status, uint16_t handle);
	bool onDisconnection(uint8_t status, uint8_t reason);
	uint16_t getConnectionHandle();

	DeviceState getState();

protected:
	HciDev *mHciDev;
	BLEAddress mAddress;
	bool mConnected;
	uint16_t mHandle;
	BLEGATT mGatt;
	DeviceState mState;
};

} /* namespace BLE */

#endif /* BLEDEVICE_H_ */
