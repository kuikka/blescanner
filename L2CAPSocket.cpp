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

L2CAPSocket::L2CAPSocket(BLEGATT *gatt, MainLoop *mainloop)
: Socket(-1, mainloop), mGatt(gatt)
{
	//	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);
}

L2CAPSocket::~L2CAPSocket()
{
}

bool L2CAPSocket::connect(const BLEAddress &address)
{
	struct sockaddr_l2 addr;

	//	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);

	int fd = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (fd < 0) {
		std::cout << "Could not get l2cap socket: " << ::strerror(errno) << "\n";
		return false;
	}

	::memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	addr.l2_bdaddr = address.address;
	addr.l2_cid = 4;
	addr.l2_bdaddr_type = address.type == BLEAddress::PUBLIC ?
			BDADDR_LE_PUBLIC : BDADDR_LE_RANDOM;

	int ret = ::connect(fd, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0) {
		std::cout << "Could not connect l2cap socket: " << ::strerror(errno) << "\n";
		::close(fd);
		return false;
	}
	setFd(fd);
	return true;
}

bool L2CAPSocket::wantToWrite()
{
	return mGatt->wantToWrite();
}

bool L2CAPSocket::wantToRead()
{
	return true;
}

bool L2CAPSocket::onPollIn()
{
	return mGatt->onPollIn();
}

bool L2CAPSocket::onPollOut()
{
	return mGatt->onPollOut();
}

bool L2CAPSocket::onPollError()
{
	return false;
}


} /* namespace BLE */
