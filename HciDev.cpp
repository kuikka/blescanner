/*
 * HciDev.cpp
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#include <iostream>
#include <stddef.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>

#if 0
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#endif

#include "MainLoop.h"
#include "HciDev.h"
#include "HciRequest.h"
#include "hci.h"
#include "BLEDevice.h"

namespace BLE {

HciDev::HciDev(int devId, MainLoop *loop) : mDevId(devId), mSocket(NULL), mLoop(loop), mCurrentRequest(NULL)
{
}

bool HciDev::open()
{
	struct sockaddr_hci addr;
	int fd, ret;

	// Open
	fd = ::socket(AF_BLUETOOTH, SOCK_RAW | SOCK_CLOEXEC, BTPROTO_HCI);
	if (fd < 0) {
		std::cout << "Could not get HCI socket: " << ::strerror(errno) << "\n";
		return false;
	}

	// Bind
	memset(&addr, 0, sizeof(addr));
	addr.hci_family = AF_BLUETOOTH;
	addr.hci_dev = mDevId;

	ret = ::bind(fd, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0) {
		std::cout << "Could not bind HCI socket: " << ::strerror(errno) << "\n";
		::close(fd);
		return false;
	}
	mSocket = new HciSocket(fd, mLoop, this);
	mSocket->setFilter();
	return true;
}

HciDev::~HciDev()
{
	delete mSocket;
}

bool HciDev::devInfo()
{
	struct hci_dev_info di;

	memset(&di, 0, sizeof(di));
	di.dev_id = mDevId;

	int ret = mSocket->ioctl(HCIGETDEVINFO, &di);
	if (ret != 0) {
		return false;
	}

	printf("HCI device\n"
			"    Name: %s\n"
			"    Flags: 0x%08x\n"
			"    Type: 0x%02x\n",
			di.name, di.flags, di.type);
#if 0
	std::cout << "HCI device\n";
	std::cout << "    Name: " << di.name << "\n";
	std::cout << "    Flags: " << di.flags << "\n";
	std::cout << "    Type: " << di.type << "\n";
#endif
	return true;
}

bool HciDev::leSetScanParameters(ScanType scanType,
		uint16_t scanInterval,
		uint16_t scanWindow,
		AddressType ownAddressType,
		ScanningFilterPolicy filterPolicy)
{

	std::cout << "leSetScanParameters(" << scanType << ", "
			<< scanInterval << ", "
			<< scanWindow << ", "
			<< ownAddressType << ", "
			<< filterPolicy << ")\n";

	HciRequest *req = new HciRequest(HCI_OP_LE_SET_SCAN_PARAMETERS);
	le_set_scan_parameters params;

	memset(&params, 0, sizeof(params));
	params.type = scanType;
	params.interval = scanInterval;
	params.window = scanWindow;
	params.own_bdaddr_type = ownAddressType;
	params.filter = filterPolicy;

	req->setCmdData(&params, sizeof(params));
	req->setExpectedReplyLength(1);

	submit(req);
}

bool HciDev::leScanEnable(bool enable, bool filterDuplicates)
{
	std::cout << "leScanEnable(" << enable << ", "
			<< filterDuplicates << ")\n";


	HciRequest *req = new HciRequest(HCI_OP_LE_SET_SCAN_ENABLE);
	le_set_scan_enable params;
	uint8_t reply = 42;

	params.enable = enable ? 1 : 0;
	params.filter_dup = filterDuplicates ? 1 : 0;

	req->setCmdData(&params, sizeof(params));
	req->setExpectedReplyLength(1);

	submit(req);
}

HciSocket* HciDev::getSocket()
{
	return mSocket;
}

void HciDev::setScanListener(BLEScanListener *listener)
{
	mScanListener = listener;
}

bool HciDev::submit(HciRequest *req)
{
	std::cout << __PRETTY_FUNCTION__ << "\n";
	mRequestQueue.push(req);

	if (mCurrentRequest == NULL) {
		mCurrentRequest = mRequestQueue.front();
		mRequestQueue.pop();
	}
}

// HciSocket functions

bool HciDev::wantToWrite()
{
	if (mCurrentRequest)
		return mCurrentRequest->wantToWrite();
}

bool HciDev::wantToRead()
{
	return true;
}

bool HciDev::onPollIn()
{
	readFromSocket();
}

bool HciDev::onPollOut()
{
	mCurrentRequest->writeToSocket(mSocket);

}

bool HciDev::onPollError()
{
	std::cout << __PRETTY_FUNCTION__ << "\n";

}

bool HciDev::readFromSocket()
{
//	std::cout << __PRETTY_FUNCTION__ << "\n";

	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	hci_event_hdr *hdr;

	int to = 10000;

	evt_cmd_complete *cc;
	evt_cmd_status *cs;
//	evt_remote_name_req_complete *rn;
	evt_le_meta_event *me;
//	remote_name_req_cp *cp;
	int len;

	len = mSocket->read(buf, sizeof(buf), true);

	hdr = (hci_event_hdr *) (buf + 1);
	ptr = buf + (1 + sizeof(hci_event_hdr));
	len -= (1 + sizeof(hci_event_hdr));

	printf("Got HCI event, evt=0x%02x, plen=0x%02x buf[0]=0x%02x\n",
			hdr->evt, hdr->plen, buf[0]);

	switch (hdr->evt) {

	case EVT_CMD_STATUS:
		/*
			   The Command Status event is used to indicate that the command described by
				the Command_Opcode parameter has been received, and that the Controller
				is currently performing the task for this command.
		 */
		cs = (evt_cmd_status *) ptr;
		printf("Got EVT_CMD_STATUS, ncmd=0x%02x, opcode=0x%04x\n",
				cs->ncmd, cs->opcode);

#if 0
		if (cs->opcode != mOpcode)
			continue;

		if (mEvent != EVT_CMD_STATUS) {
			if (cs->status) {
				goto failed;
			}
			break;
		}

		memcpy(mReplyData, ptr, mReplyDataSize);
#endif
		break;

	case EVT_CMD_COMPLETE:
		/* Command was completed */
		cc = (evt_cmd_complete *) ptr;

		printf("Got EVT_CMD_COMPLETE, ncmd=0x%02x, opcode=0x%04x\n",
				cc->ncmd, cc->opcode);

		ptr += sizeof(evt_cmd_complete);
		len -= sizeof(evt_cmd_complete);

		if (mCurrentRequest->getOpcode() == cc->opcode) {
			completeCurrentRequest(ptr, len);
		}

		break;

		#define EVT_LE_ADVERTISING_REPORT 0x02

	case EVT_LE_META_EVENT:
		/* LE meta event */
		me = (evt_le_meta_event *) ptr;
		len -= sizeof(*me);
		printf("Got EVT_LE_META_EVENT subevent=0x%02x\n", me->subevent);
		switch (me->subevent) {
		case EVT_LE_ADVERTISING_REPORT:
			handleAdvertisingReport(me->data, len);
			break;
		}

		break;

	default:
		printf("Got default\n");
		break;
	}
}

void HciDev::handleAdvertisingReport(const uint8_t *data, size_t len)
{
	if (mScanListener) {

//		mScanListener->onAdvertisingReport(me[1]);
	}
}

void HciDev::completeCurrentRequest(const uint8_t *ptr, size_t len)
{
	mCurrentRequest->complete(ptr, len);
	delete mCurrentRequest;
	mCurrentRequest = NULL;
	if (mRequestQueue.size() > 0) {
		mCurrentRequest = mRequestQueue.front();
		mRequestQueue.pop();
	}
}

bool HciDev::connect(BLEDevice *dev)
{

}

}; // namespace BLE
