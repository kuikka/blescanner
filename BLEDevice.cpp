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
	mHciDev->leConnect(this);
}

bool BLEDevice::disconnect()
{
	mHciDev->leDisconnect(this);
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
		mConnected = true;
		mHandle = handle;
		printf("Got connected! Handle=%d\n", handle);
		mGatt.connect();
		mGatt.discoverAll();
	}
}

bool BLEDevice::onDisonnection(uint8_t reason)
{
	mConnected = false;
	mHandle = 0xFFFF;
}

} /* namespace BLE */
