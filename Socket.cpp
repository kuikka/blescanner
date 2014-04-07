/*
 * Socket.cpp
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <iostream>

#include "Socket.h"
#include "MainLoop.h"

Socket::Socket(int fd, MainLoop *loop) : mFd(fd), mLoop(loop)
{
	if (mLoop && fd >= 0)
		loop->addSocket(this);
}

Socket::~Socket()
{
	::close(mFd);
	if (mLoop)
		mLoop->removeSocket(this);
}


void Socket::setFd(int fd)
{
	mFd = fd;
	if (mFd >= 0 && mLoop)
		mLoop->addSocket(this);
}

int Socket::write(const void *buf, size_t count)
{
	ssize_t len;
	size_t written = 0;
	const uint8_t *ptr = (const uint8_t*) buf;

	while (written < count) {
		len = ::write(mFd, ptr, count);
		if (len < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			return -errno;
		} else {
			written += len;
			ptr += len;
		}
	}
	return written;
}

int Socket::read(void *buf, size_t count, bool single)
{
	ssize_t len;
	size_t bytesRead = 0;
	uint8_t *ptr = (uint8_t*)buf;

	while (bytesRead < count) {
		len = ::read(mFd, ptr, count);
		if (len < 0) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			return -errno;
		} else {
			bytesRead += len;
			ptr += len;
			if (single)
				return len;
		}
	}
	return bytesRead;
}

int Socket::ioctl(int request, void *buf)
{
	int ret = ::ioctl(mFd, request, buf);
	if (ret < 0) {
		return errno;
	}
	return ret;
}


int Socket::getOpt(int level, int optname, void *optval, unsigned int *optlen)
{
	int ret;

	ret = ::getsockopt(mFd, level, optname, optval, optlen);
	if (ret < 0)
		return -errno;
	return 0;
}

int Socket::setOpt(int level, int optname, const void *optval, unsigned int optlen)
{
	int ret;

	ret = ::setsockopt(mFd, level, optname, optval, optlen);
	if (ret < 0)
		return -errno;
	return 0;
}

int Socket::poll(unsigned int events, int timeout)
{
	struct pollfd fds;

	fds.fd = mFd;
	fds.events = 0;
	if (events & Socket::PollIn)
		fds.events |= POLLIN;
	if (events & Socket::PollOut)
		fds.events |= POLLOUT;

	while(1) {
		int ret = ::poll(&fds, 1, timeout);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			}
			return -errno;
		} else if (ret == 0) {
			std::cout << "Poll timeout\n";
			return -1;
		} else {
			return 0;
		}

	}

}

bool Socket::close()
{
	::close(mFd);
	mFd = -1;
}
