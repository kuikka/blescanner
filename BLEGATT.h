/*
 * BLEGATT.h
 *
 *  Created on: Mar 27, 2014
 *      Author: kuikka
 */

#ifndef BLEGATT_H_
#define BLEGATT_H_

#include <map>
#include "L2CAPSocket.h"
#include "UUID.h"

namespace BLE {

class BLEDevice;

typedef struct
{
	uint16_t				handle;
	UUID 					type;
	std::vector<uint8_t>	value;
	uint16_t				last_handle_in_group;
} ATT_entry;

typedef std::map<uint16_t, ATT_entry> ATTMap;

class BLEGATT
{
public:
	BLEGATT(BLEDevice *dev);
	virtual ~BLEGATT();
	bool connect();
	bool disconnect();
	bool discoverAll();

protected:
	bool opReadByGroupTypeReq(uint16_t startHandle, uint16_t endHandle, const UUID & uuid);
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
	ATTMap mATT;
	UUID mReqType;

	friend class L2CAPSocket;
};

} /* namespace BLE */

#endif /* BLEGATT_H_ */
