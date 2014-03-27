/*
 * BLEDevice.h
 *
 *  Created on: Mar 26, 2014
 *      Author: kuikka
 */

#ifndef BLEDEVICE_H_
#define BLEDEVICE_H_

#include "BLEAddress.h"

namespace BLE {

class BLEDevice
{
public:
	BLEDevice();
	BLEDevice(const BLEAddress& bdaddr);
	virtual ~BLEDevice();

protected:
	BLEAddress mAddress;
};

} /* namespace BLE */

#endif /* BLEDEVICE_H_ */
