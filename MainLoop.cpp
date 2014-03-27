/*
 * MainLoop.cpp
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#include <unistd.h>
#include <vector>
#include <poll.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include "MainLoop.h"
#include "Socket.h"

MainLoop::MainLoop()
{
	// TODO Auto-generated constructor stub

}

MainLoop::~MainLoop()
{
	// TODO Auto-generated destructor stub
}

bool MainLoop::run()
{
	std::cout << __PRETTY_FUNCTION__  << "\n";

	int ret;
	char cmd;
	bool running = true;
	int timeout_ms = -1;
	std::vector<struct pollfd> fds;

	ret = pipe(mPipe);
	if (ret < 0)
		return false;

	while(running)
	{
		unsigned int nr_sockets = mSockets.size() + 1;
		unsigned i = 0;

		fds.resize(nr_sockets);

		fds[i].fd = mPipe[0];
		fds[i].events = POLLIN;

		i++;

		for (SocketsData::iterator it = mSockets.begin();
				it != mSockets.end();
				++it)
		{
			Socket *s = it->second;

			fds[i].events = POLLERR;

			fds[i].fd = s->getFd();

			if (s->wantToWrite())
				fds[i].events |= POLLOUT;

			if (s->wantToRead())
				fds[i].events |= POLLIN;

#if 0
			printf("poll fd=%d events=%x [%s %s %s]\n", fds[i].fd, fds[i].events,
					fds[i].events & POLLIN ? "POLLIN" : "",
					fds[i].events & POLLOUT ? "POLLOUT" : "",
					fds[i].events & POLLERR ? "POLLERR" : "");
#endif
			i++;
		}

//		printf("Doing poll %d\n", nr_sockets);
		ret = ::poll(fds.data(), nr_sockets, timeout_ms);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
		} else if (ret == 0) {
			// timeout
		} else {
			// Handle intenal event
			if (fds[0].revents & POLLIN) {
				ret = ::read(mPipe[0], &cmd, 1);
				if (ret == 1) {
					switch (cmd) {
					case 'n':
						// New socket or read/write updated
						continue;
						break;
					}
				}
			}
			// Handle the rest
			for (i = 1; i < nr_sockets; ++i) {
#if 0
				printf("poll fd=%d revents=%x [%s %s %s]\n", fds[i].fd, fds[i].revents,
						fds[i].revents & POLLIN ? "POLLIN" : "",
						fds[i].revents & POLLOUT ? "POLLOUT" : "",
						fds[i].revents & POLLERR ? "POLLERR" : "");
#endif
				if (fds[i].revents & POLLERR)
					mSockets[fds[i].fd]->onPollError();
				if (fds[i].revents & POLLIN)
					mSockets[fds[i].fd]->onPollIn();
				if (fds[i].revents & POLLOUT)
					mSockets[fds[i].fd]->onPollOut();
			}
		}
	}

}

bool MainLoop::addSocket(Socket *sock)
{
	mSockets[sock->getFd()] = sock;
}

bool MainLoop::removeSocket(Socket *sock)
{
	mSockets.erase(sock->getFd());
}


bool MainLoop::notifyLoop()
{
	char cmd = 'n';

	int ret = write(mPipe[1], &cmd, sizeof(cmd));
	if (ret != 1)
		return false;
	return true;
}
