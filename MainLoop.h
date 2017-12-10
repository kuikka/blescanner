/*
 * MainLoop.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef MAINLOOP_H_
#define MAINLOOP_H_

#include <map>
#include <stdint.h>
class Socket;

class TimeoutCallback
{
	virtual void onTimeout(void *opaque) = 0;
};

class MainLoop
{
public:
	MainLoop();
	virtual bool run();
	virtual bool addSocket(Socket *sock);
	virtual bool removeSocket(Socket *sock);
	virtual bool setTimeout(uint32_t timeoutMs, TimeoutCallback *cb);

	virtual ~MainLoop();
	virtual bool notifyLoop();

protected:
	typedef std::map<int, Socket*> SocketsData;
	SocketsData mSockets;
	int mPipe[2];
};

#endif /* MAINLOOP_H_ */
