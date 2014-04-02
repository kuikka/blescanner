/*
 * BLEGATT.h
 *
 *  Created on: Mar 27, 2014
 *      Author: kuikka
 */

#ifndef BLEGATT_H_
#define BLEGATT_H_

#include "L2CAPSocket.h"

namespace BLE {

class BLEDevice;

class BLEGATT
{
public:
	BLEGATT(BLEDevice *dev);
	virtual ~BLEGATT();
	bool connect();
	bool discoverAll();

protected:
	L2CAPSocket mL2CAP;
	BLEDevice *mDev;
};

} /* namespace BLE */

#endif /* BLEGATT_H_ */
