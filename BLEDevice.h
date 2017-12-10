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
	enum class State {
		DISCONNECTED,
		CONNECTING,
		CONNECTED,
		DISCONNECTING,
	};

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

	State getState();

protected:
	HciDev *mHciDev = nullptr;
	BLEAddress mAddress;
	bool mConnected = false;
	BLEGATT mGatt;
	uint16_t mHandle = 0xFFFF;
	State mState = State::DISCONNECTED;
};

} /* namespace BLE */

#endif /* BLEDEVICE_H_ */
