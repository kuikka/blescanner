/*
 * HciRequest.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef HCIREQUEST_H_
#define HCIREQUEST_H_

#include <stdint.h>
#include <unistd.h>
#include "hci.h"

class Socket;

namespace BLE {

class HciDev;
class HciSocket;

class HciRequest
{
public:
	HciRequest(uint16_t opcode);
	virtual ~HciRequest();

	void setCmdData(const void *data, size_t count);
	void setExpectedReplyLength(size_t count);
	bool executeSync(HciDev *dev);

	uint16_t getOpcode() { return mOpcode; };
	void complete(const uint8_t *reply, size_t len);

protected:
	bool getReply(Socket *sock);

public:
	virtual bool wantToWrite();
	virtual bool wantToRead();
	virtual bool writeToSocket(HciSocket *sock);

protected:
	uint16_t mOpcode;
	int mEvent;
	hci_cmd mCmd;
//	uint8_t *mCmdData;
//	size_t mCmdDataSize;
	void *mReplyData;
	size_t mExpectedReplyDataSize;
	size_t mReplyDataSize;
	bool mReqWritten;
	typedef void (*CompleteCallback) (const uint8_t * ptr, size_t len);
	CompleteCallback mCompleteCallback;
};

}; // namespace BLE

#endif /* HCIREQUEST_H_ */
