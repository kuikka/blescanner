/*
 * L2CAPSocket.cpp
 *
 *  Created on: Mar 27, 2014
 *      Author: kuikka
 */
#include <string.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <iostream>
#include <errno.h>
#include <stdio.h>

#include "L2CAPSocket.h"
#include "BLEDevice.h"
#include "l2cap.h"

namespace BLE {

L2CAPSocket::L2CAPSocket(BLEDevice *bleDev)
	: Socket(-1, bleDev->getHciDev()->getMainLoop()), mDev(bleDev)
{
	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);
}

L2CAPSocket::~L2CAPSocket()
{
}

bool L2CAPSocket::connect()
{
	struct sockaddr_l2 addr;

	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);

	mFd = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (mFd < 0) {
		std::cout << "Could not get l2cap socket: " << ::strerror(errno) << "\n";
		return false;
	}

	::memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	addr.l2_bdaddr = mDev->getAddress().address;
	addr.l2_cid = 4;
	addr.l2_bdaddr_type = mDev->getAddress().type == BLEAddress::PUBLIC ?
			BDADDR_LE_PUBLIC : BDADDR_LE_RANDOM;

	int ret = ::connect(mFd, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0) {
		std::cout << "Could not connect l2cap socket: " << ::strerror(errno) << "\n";
		::close(mFd);
		mFd = -1;
		return false;
	}
	return true;
}

bool L2CAPSocket::wantToWrite()
{

}

bool L2CAPSocket::wantToRead()
{

}

bool L2CAPSocket::onPollIn()
{

}

bool L2CAPSocket::onPollOut()
{

}

bool L2CAPSocket::onPollError()
{

}


} /* namespace BLE */
