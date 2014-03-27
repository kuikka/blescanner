/*
 * HciSocket.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef REQUESTSOCKET_H_
#define REQUESTSOCKET_H_

#include <list>
#include "Socket.h"

class MainLoop;

namespace BLE {

class HciDev;

class HciSocket: public Socket
{
public:
	HciSocket(int fd, MainLoop *loop, HciDev *dev);
	virtual ~HciSocket();
	bool setFilter();

public: // Inherited from Socket
	virtual bool wantToWrite();
	virtual bool wantToRead();
	virtual bool onPollIn();
	virtual bool onPollOut();
	virtual bool onPollError();

protected:
	HciDev *mDev;

};

};
#endif /* REQUESTSOCKET_H_ */
