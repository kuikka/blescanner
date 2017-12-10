/*
 * BLEDevice.cpp
 *
 *  Created on: Mar 26, 2014
 *      Author: kuikka
 */
#include <stdio.h>
#include "BLEDevice.h"
#include "utils.h"

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
	mState = State::CONNECTING;
	return mHciDev->leConnectViaWhiteList(this);
}

bool BLEDevice::disconnect()
{
	mState = State::DISCONNECTING;
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
		mState = State::CONNECTED;
		mConnected = true;
		mHandle = handle;
		mGatt.connect();
		mGatt.discoverAll();
	}
	return true;
}

bool BLEDevice::onDisconnection(uint8_t status, uint8_t reason)
{
	std::cout << "BLEDevice: got disconnected status=" << hex(status) << " reason=" << hex(reason) << std::endl;
	mConnected = false;
	mHandle = 0xFFFF;
	mGatt.disconnect();
	mState = State::DISCONNECTED;
	return true;
}

BLEDevice::State BLEDevice::getState()
{
	return mState;
}

} /* namespace BLE */

