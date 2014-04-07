/*
 * BLEDevice.cpp
 *
 *  Created on: Mar 26, 2014
 *      Author: kuikka
 */
#include <stdio.h>
#include "BLEDevice.h"

namespace BLE {

BLEDevice::BLEDevice(HciDev *hciDev, const BLEAddress& bdaddr)
: mHciDev(hciDev), mAddress(bdaddr), mConnected(false),
  mGatt(this), mHandle(0xFFFF)
{
	mHciDev->addDevice(this);
}

BLEDevice::~BLEDevice()
{
	if (mConnected)
		disconnect();
	mHciDev->removeDevice(this);
}

bool BLEDevice::connect()
{
	mState = CONNECTING;
	return mHciDev->leConnect(this);
}

bool BLEDevice::disconnect()
{
	mState == DISCONNECTING;
	return mHciDev->leDisconnect(this);
}

uint16_t BLEDevice::getConnectionHandle()
{
	return mHandle;
}


BLEAddress& BLEDevice::getAddress()
{
	return mAddress;
}

// Called when connection to device has been established
bool BLEDevice::onConnection(uint8_t status, uint16_t handle)
{
	if (status == 0) {
		printf("BLEDevice: Got connected! Handle=%d\n", handle);
		mState = CONNECTED;
		mConnected = true;
		mHandle = handle;
		mGatt.connect();
		mGatt.discoverAll();
	}
	return true;
}

bool BLEDevice::onDisconnection(uint8_t status, uint8_t reason)
{
	std::cout << "BLEDevice got disconnected\n";
	mConnected = false;
	mHandle = 0xFFFF;
	mGatt.disconnect();
	mState = DISCONNECTED;
	return true;
}

BLEDevice::DeviceState BLEDevice::getState()
{
	return mState;
}

} /* namespace BLE */

