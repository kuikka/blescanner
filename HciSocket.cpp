/*
 * HciSocket.cpp
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */
#if 0
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#endif

#include <string.h>
#include "HciSocket.h"
#include "HciDev.h"

namespace BLE {

HciSocket::HciSocket(int fd, MainLoop *loop, HciDev *dev) : Socket(fd, loop), mDev(dev)
{
}

HciSocket::~HciSocket()
{
}

bool HciSocket::setFilter()
{
	struct hci_filter nf, of;

	memset(&nf, 0, sizeof(nf));
	memset(&nf.event_mask, 0xff, sizeof(nf.event_mask));
	memset(&nf.type_mask, 0xff, sizeof(nf.type_mask));
#if 0
	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT,  &nf);
	hci_filter_set_event(EVT_CMD_STATUS, &nf);
	hci_filter_set_event(EVT_CMD_COMPLETE, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
#endif
	//	hci_filter_set_opcode(0x200b, &nf);

	if (this->setOpt(SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
		return false;

	return true;
}

bool HciSocket::wantToWrite()
{
	return mDev->wantToWrite();
}

bool HciSocket::wantToRead()
{
	return mDev->wantToRead();
}

bool HciSocket::onPollIn()
{
	return mDev->onPollIn();
}

bool HciSocket::onPollOut()
{
	mDev->onPollOut();
}

bool HciSocket::onPollError()
{
	mDev->onPollError();
}

} // namespace BLE
