/*
 * Socket.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <unistd.h>

class MainLoop;

class Socket {
public:
	typedef enum {
		PollIn = 1,
		PollOut = 2,
	} PollEvents;

public:
	Socket(int fd = -1, MainLoop *loop = NULL);
	virtual ~Socket();
	void setFd(int fd);

	int write(const void *buf, size_t count);
	int read(void *buf, size_t count, bool single);
	int ioctl(unsigned long request, void *buf);
	int getOpt(int level, int optname, void *optval, unsigned int *optlen);
	int setOpt(int level, int optname, const void *optval, unsigned int optlen);
	int poll(unsigned int events, int timeout);
	bool close();

public:
	int getFd() { return mFd; };

public:
	virtual bool wantToWrite() = 0;
	virtual bool wantToRead() = 0;
	virtual bool onPollIn() = 0;
	virtual bool onPollOut() = 0;
	virtual bool onPollError() = 0;

protected:
	int mFd;
	MainLoop *mLoop;
};

#endif /* SOCKET_H_ */
