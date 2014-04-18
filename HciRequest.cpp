/*
 * HciRequest.cpp
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#include <stdint.h>
#include <cstring>
#include <iostream>
#include <strings.h>

#include "HciRequest.h"
#include "HciDev.h"
#include "HciSocket.h"

#if 0
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#endif

namespace BLE {

HciRequest::HciRequest(uint16_t opcode) : mOpcode(opcode), mEvent(0),
		mExpectedReplyDataSize(0), mReplyDataSize(0), mReplyData(NULL), mReqWritten(false),
		mCompleteCallback(NULL)
{
	::bzero(&mCmd, sizeof(mCmd));
}

HciRequest::~HciRequest()
{
}

void HciRequest::setCmdData(const void *data, size_t count)
{
	if (count > sizeof(mCmd.data)) {
		return;
	}
	::memcpy(mCmd.data, data, count);
	mCmd.dlen = count;
//	mCmdData = new uint8_t[count];
//	memcpy(mCmdData, data, count);
//	mCmdDataSize = count;
}

void HciRequest::setExpectedReplyLength(size_t count)
{
	mExpectedReplyDataSize = count;
}


bool HciRequest::executeSync(HciDev *dev)
{
}

bool HciRequest::wantToWrite()
{
	return !mReqWritten;
}

bool HciRequest::wantToRead()
{
	return true;
}

bool HciRequest::writeToSocket(HciSocket *sock)
{
	mCmd.type = HCI_COMMAND_PKT;
	mCmd.opcode = mOpcode;

	int ret = sock->write(&mCmd, 4 + mCmd.dlen);

	if (ret < 0) {
		std::cout << "HciRequest::writeToSocket: Failed to send command\n";
		return false;
	}
//	std::cout << "Wrote " << ret << " bytes\n";
	mReqWritten = true;
}

void HciRequest::complete(const uint8_t *reply, size_t len)
{
	if (mCompleteCallback)
		mCompleteCallback(reply, len);
}


}; // namespace BLE
