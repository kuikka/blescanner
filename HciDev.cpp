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

#include "MainLoop.h"
#include "HciDev.h"
#include "HciRequest.h"
#include "hci.h"
#include "BLEDevice.h"
#include "utils.h"

namespace BLE {

HciDev::HciDev(int devId, MainLoop *loop)
: mSocket(nullptr), mDevId(devId), mLoop(loop),
  mCurrentRequest(nullptr), mScanning(false), mConnecting(false), mWhiteListSize(0)
{
	printf("%s this=%p\n", __PRETTY_FUNCTION__, (void*)this);
}

HciDev::~HciDev()
{
	if (mScanning) {
		leScanEnable(false, false);
	}
	delete mSocket;
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
	//addr.hci_channel = 0; // raw

	ret = ::bind(fd, (struct sockaddr *) &addr, sizeof(addr));
	if (ret < 0) {
		std::cout << "Could not bind HCI socket: " << ::strerror(errno) << " " << mDevId << "\n";
		::close(fd);
		return false;
	}
	mSocket = new HciSocket(fd, mLoop, this);
	mSocket->setFilter();
	leReadBufferSize();
	leReadWhiteListSize();
	return true;
}

bool HciDev::up()
{
	int ret = ::ioctl(mSocket->getFd(), HCIDEVUP, mDevId);
	if (ret < 0) {
		std::cout << "Could not bind HCI device " << mDevId << " up: " << ::strerror(errno) << "\n";
		return false;
	}
	return true;
}

bool HciDev::down()
{
	int ret = ::ioctl(mSocket->getFd(), HCIDEVDOWN, mDevId);
	if (ret < 0) {
		std::cout << "Could not bind HCI device " << mDevId << " down: " << ::strerror(errno) << "\n";
		return false;
	}
	return true;
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
		BLEAddress::AddressType ownAddressType,
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

	return submit(req);
}

bool HciDev::leAddToWhitelist(const BLEAddress &addr)
{
	HciRequest *req = new HciRequest(HCI_OP_LE_ADD_TO_WHITE_LIST);

	le_add_to_white_list data;
	memset(&data, 0, sizeof(data));

	data.peer_bdaddr_type = addr.type;
	data.peer_bdaddr = addr.address;
	req->setCmdData(&data, sizeof(data));

	return submit(req);
}

bool HciDev::leScanEnable(bool enable, bool filterDuplicates)
{
	std::cout << "leScanEnable(" << enable << ", "
			<< filterDuplicates << ")\n";


	HciRequest *req = new HciRequest(HCI_OP_LE_SET_SCAN_ENABLE);
	le_set_scan_enable params;

	params.enable = enable ? 1 : 0;
	params.filter_dup = filterDuplicates ? 1 : 0;

	req->setCmdData(&params, sizeof(params));
	req->setExpectedReplyLength(1);

	mScanning = enable;
	return submit(req);
}

HciSocket* HciDev::getSocket()
{
	return mSocket;
}

void HciDev::setScanListener(std::shared_ptr<BLEScanListener> listener)
{
	mScanListeners.push_back(listener);
}

bool HciDev::submit(HciRequest *req)
{
#if 0
	printf("HciDev: submitted 0x%04x. Queue size before: %lu. Current request = %p\n", req->getOpcode(),
			mRequestQueue.size(), mCurrentRequest);
#endif

	mRequestQueue.push(req);

	if (mCurrentRequest == NULL) {
		mCurrentRequest = mRequestQueue.front();
		mRequestQueue.pop();
	}
	return true;
}

// HciSocket functions

bool HciDev::wantToWrite()
{
	if (mCurrentRequest)
		return mCurrentRequest->wantToWrite();
	return false;
}

bool HciDev::wantToRead()
{
	return true;
}

bool HciDev::onPollIn()
{
	return readFromSocket();
}

bool HciDev::onPollOut()
{
	return mCurrentRequest->writeToSocket(mSocket);
}

bool HciDev::onPollError()
{
	std::cout << __PRETTY_FUNCTION__ << "\n";
	return false;
}

bool HciDev::readFromSocket()
{
	//	std::cout << __PRETTY_FUNCTION__ << "\n";

	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;

	//	remote_name_req_cp *cp;
	int len;

	len = mSocket->read(buf, sizeof(buf), true);

	switch(buf[0])
	{
		case HCI_EVENT_PKT:
		{
			hci_event_hdr *hdr;

			hdr = (hci_event_hdr *) (buf + 1);
			ptr = buf + (1 + sizeof(hci_event_hdr));
			len -= (1 + sizeof(hci_event_hdr));

#if 0
			printf("Got HCI event, evt=0x%02x, plen=0x%02x buf[0]=0x%02x\n",
					hdr->evt, hdr->plen, buf[0]);
#endif

			switch (hdr->evt) {

				case EVT_CMD_STATUS:
				{
					/*
				   The Command Status event is used to indicate that the command described by
					the Command_Opcode parameter has been received, and that the Controller
					is currently performing the task for this command.
					 */
					evt_cmd_status *cs = (evt_cmd_status *) ptr;
#if 0
					printf("Got EVT_CMD_STATUS, status=0x%02x, ncmd=0x%02x, opcode=0x%04x\n",
							cs->status, cs->ncmd, cs->opcode);
#endif
					onCommandStatus(cs->opcode, cs->status);

				}
				break;

				case EVT_CMD_COMPLETE:
				{
					/* Command was completed */
					evt_cmd_complete *cc = (evt_cmd_complete *) ptr;

#if 0
					printf("Got EVT_CMD_COMPLETE, ncmd=0x%02x, opcode=0x%04x\n",
							cc->ncmd, cc->opcode);
#endif

					ptr += sizeof(evt_cmd_complete);
					len -= sizeof(evt_cmd_complete);

					if (mCurrentRequest && mCurrentRequest->getOpcode() == cc->opcode) {
						completeCurrentRequest(ptr, len);
					}
					onCommandCompleted(cc->opcode, ptr, len);
				}
				break;

				case EVT_DISCONN_COMPLETE:
				{
					evt_disconnection_complete *dis = (evt_disconnection_complete*) ptr;
					onDisconnectionEvent(dis->status, dis->handle, dis->reason);
				}
				break;

				case EVT_LE_META_EVENT:
				{
					/* LE meta event */
					evt_le_meta_event *me = (evt_le_meta_event *) ptr;
					len -= sizeof(*me);
//					printf("Got EVT_LE_META_EVENT subevent=0x%02x\n", me->subevent);
					onLEMetaEvent(me->subevent, me->data, len);
				}
				break;

				default:
///					printf("Got default\n");
					break;
			}
		}
		break;

		case HCI_COMMAND_PKT:
		{
			//printf("Got HCI cmd\n");
		}
		break;
	}
	return true;

}

void HciDev::handleAdvertisingReport(const uint8_t *data, size_t /* len */)
{
//	printf("%s\n", __FUNCTION__);

	if (mScanListeners.empty())
		return;

	unsigned num_reports = data[0];
	const uint8_t *ptr = &data[1];

	// TODO: Use length, write parser

	while (num_reports--) {
		BLEScanListener::EventType type = (BLEScanListener::EventType) *ptr++;
		BLEAddress::AddressType addrType = (BLEAddress::AddressType) *ptr++;
		BLE::BLEAddress from(ptr, addrType);
		ptr += 6;
		size_t length = *ptr++;
		const uint8_t *dataPtr = (const uint8_t *)ptr;
		ptr += length;
		int8_t rssi = (int8_t) *((int8_t*)ptr);

		for (auto scanListener : mScanListeners) {
			scanListener->onAdvertisingReport(*this, type, from, rssi, dataPtr, length);
		}
	}
}

void HciDev::completeCurrentRequest(const uint8_t *ptr, size_t len)
{
//	printf("HciDev: completed 0x%04x\n", mCurrentRequest->getOpcode());

	mCurrentRequest->complete(ptr, len);
	delete mCurrentRequest;
	mCurrentRequest = NULL;
	if (mRequestQueue.size() > 0) {
		mCurrentRequest = mRequestQueue.front();
		mRequestQueue.pop();
	}
}

bool HciDev::leConnect(BLEDevice *dev, bool useWhiteList)
{
	HciRequest *req = new HciRequest(HCI_OP_LE_CREATE_CONN);

	le_create_connection conn;

	::memset(&conn, 0, sizeof(conn));

	conn.scan_interval = 4;
	conn.scan_window = 4;
	conn.min_interval = 0x001F;
	conn.max_interval = 0x004F;
	conn.supervision_timeout = 0xc80;
	conn.min_ce_length = 1;
	conn.max_ce_length = 1;
	conn.latency = 8;

	if (useWhiteList) {
		conn.initiator_filter = 1;
	} else {
		::memcpy(&conn.peer_bdaddr.b, &dev->getAddress().address.b, 6);
		conn.peer_bdaddr_type = dev->getAddress().type;
	}

	req->setCmdData(&conn, sizeof(conn));
	req->setExpectedReplyLength(1);

	return submit(req);
}

bool HciDev::leConnectViaWhiteList(BLEDevice *dev)
{
	if (mConnecting) {
		leCancelConnection();
	}
	leAddToWhitelist(dev->getAddress());
	leConnect(NULL, true);

	return true;
}

bool HciDev::leDisconnect(BLEDevice *bleDev)
{
	HciRequest *req = new HciRequest(HCI_OP_DISCONNECT);

	hci_disconnect dis;
	::memset(&dis, 0, sizeof(dis));
	dis.handle = bleDev->getConnectionHandle();
	dis.reason = 0x13;
	req->setCmdData(&dis, sizeof(dis));

	return submit(req);
}


bool HciDev::leCancelConnection()
{
	HciRequest *req = new HciRequest(HCI_OP_LE_CREATE_CONN_CANCEL);
	return submit(req);
}

bool HciDev::leReadBufferSize()
{
	HciRequest *req = new HciRequest(HCI_OP_LE_READ_BUFFER_SIZE);
	return submit(req);
}

bool HciDev::leReadWhiteListSize()
{
	HciRequest *req = new HciRequest(HCI_OP_LE_READ_WHITE_LIST_SIZE);
	return submit(req);
}

bool HciDev::leClearWhiteList()
{
	HciRequest *req = new HciRequest(HCI_OP_LE_CLEAR_WHITE_LIST);
	return submit(req);

}

void HciDev::onCommandCompleted(uint16_t opCode, const uint8_t *data, size_t __attribute__((unused))dataLen)
{
//	std::cout << "Command completed: " << opCode << "\n";
	switch (opCode) {
		case HCI_OP_LE_READ_BUFFER_SIZE:
		{
			le_read_buffer_size *bs = (le_read_buffer_size*) data;
			std::cout << "HciDev: Got LE buffer size, data_packet_len=" << bs->data_packet_len << " num_data_packets=" << (unsigned)bs->nr_data_packets << "\n";
		}
		break;
		case HCI_OP_LE_READ_WHITE_LIST_SIZE:
		{
			le_read_white_list_size *wlz = (le_read_white_list_size*) data;
			std::cout << "HciDev: Got LE white list size, white_list_size=" << (unsigned)wlz->list_size << "\n";
			mWhiteListSize = wlz->list_size;
		}
		break;
		case HCI_OP_LE_CLEAR_WHITE_LIST:
		{
			std::cout << "HciDev: White list cleared\n";
		}
		break;
		case HCI_OP_LE_ADD_TO_WHITE_LIST:
		{
			std::cout << "HciDev: Added to white list status=" << hex(data[0]) << "\n";
		}
		break;
		case HCI_OP_LE_CREATE_CONN_CANCEL:
		{
			std::cout << "HciDev: Cancelled connection, status=" << hex(data[0]) << std::endl;
		}
		break;
	}
}

void HciDev::onCommandStatus(uint16_t opCode, uint8_t status)
{
	if (opCode == HCI_OP_LE_CREATE_CONN) {
		if (mCurrentRequest && mCurrentRequest->getOpcode() == opCode)
			completeCurrentRequest(&status, 1);
	}
}

void HciDev::onDisconnectionEvent(uint8_t status, uint16_t handle, uint8_t reason)
{
	printf("Handle %d disconnected\n", handle);
	BLEDevice *dev = findDeviceByHandle(handle);
	if (dev)
		dev->onDisconnection(status, reason);

}

void HciDev::onLEMetaEvent(uint8_t subEvent, const uint8_t *data, size_t datalen)
{
	switch (subEvent) {

		case EVT_LE_CONNECTION_COMPLETE:
		{
			evt_le_connection_complete *c = (evt_le_connection_complete*) data;
			BLEAddress addr(c->peer_bdaddr, (BLEAddress::AddressType) c->peer_bdaddr_type);

			std::cout << "EVT: Connection completed, status="
					<< hex(c->status)
					<< " to: " << addr << "\n";
			BLEDevice *dev = findDeviceByAddress(addr);
			if (dev)
				dev->onConnection(c->status, c->handle);
			if (c->status == 0)
				leConnect(NULL, true);

			break;
		}

		case EVT_LE_ADVERTISING_REPORT:
		{
			handleAdvertisingReport(data, datalen);
			break;
		}

	}
}

void HciDev::addDevice(BLEDevice *dev)
{
	mDevices.insert(dev);
}

void HciDev::removeDevice(BLEDevice* /* dev */)
{

}

BLEDevice* HciDev::findDeviceByHandle(uint16_t handle)
{
	for (DeviceMap::iterator i = mDevices.begin();
			i != mDevices.end();
			++i)
	{
		BLEDevice *dev = *i;
		if (dev->getConnectionHandle() == handle) {
			return dev;
		}
	}
	return NULL;
}

BLEDevice* HciDev::findDeviceByAddress(const BLEAddress &addr)
{
	for (DeviceMap::iterator i = mDevices.begin();
			i != mDevices.end();
			++i)
	{
		BLEDevice *dev = *i;
		if (dev->getAddress() == addr) {
			return dev;
		}
	}
	return NULL;
}
} // namespace BLE
