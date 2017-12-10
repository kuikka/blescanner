/*
 * L2CAPSocket.h
 *
 *  Created on: Mar 27, 2014
 *      Author: kuikka
 */

#ifndef L2CAPSOCKET_H_
#define L2CAPSOCKET_H_

#include "Socket.h"
#include "BLEAddress.h"

namespace BLE {

class BLEGATT;

class L2CAPSocket: public Socket
{
public:
	L2CAPSocket(BLEGATT *gatt, MainLoop *mainloop);
	virtual ~L2CAPSocket();
	bool connect(const BLEAddress &address);

public:
	virtual bool wantToWrite();
	virtual bool wantToRead();
	virtual bool onPollIn();
	virtual bool onPollOut();
	virtual bool onPollError();

protected:
	BLEGATT *mGatt;
};

} /* namespace BLE */

#endif /* L2CAPSOCKET_H_ */
