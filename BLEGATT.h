/*
 * BLEGATT.h
 *
 *  Created on: Mar 27, 2014
 *      Author: kuikka
 */

#ifndef BLEGATT_H_
#define BLEGATT_H_

#include "L2CAPSocket.h"
#include "UUID.h"

namespace BLE {

class BLEDevice;

class BLEGATT
{
public:
	BLEGATT(BLEDevice *dev);
	virtual ~BLEGATT();
	bool connect();
	bool disconnect();
	bool discoverAll();

protected:
	bool opReadByGroupReq(uint16_t startHandle, uint16_t endHandle, const UUID & uuid);
	bool wantToWrite();
	bool wantToRead();
	bool onPollIn();
	bool onPollOut();
	bool onPollError();
	void readFromSocket();

protected:
	L2CAPSocket mL2CAP;
	BLEDevice *mDev;
	bool mDiscovering;

	friend class L2CAPSocket;
};

} /* namespace BLE */

#endif /* BLEGATT_H_ */
