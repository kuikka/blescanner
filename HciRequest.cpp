/*
 * HciRequest.cpp
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#include <stdint.h>
#include <cstring>
#include <iostream>

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
	mCmd.opcode = mOpcode;
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

#if 0
bool HciRequest::getReply(Socket *sock)
{
	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	struct hci_filter nf, of;
	hci_event_hdr *hdr;

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT,  &nf);
	hci_filter_set_event(EVT_CMD_STATUS, &nf);
	hci_filter_set_event(EVT_CMD_COMPLETE, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
//	hci_filter_set_event(r->event, &nf);
	hci_filter_set_opcode(mOpcode, &nf);

	if (sock->setOpt(SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
		return false;

	int to = 10000;

	int tries = 10;
	while (tries--) {
		evt_cmd_complete *cc;
		evt_cmd_status *cs;
		evt_remote_name_req_complete *rn;
		evt_le_meta_event *me;
		remote_name_req_cp *cp;
		int len;

		int ret = sock->poll(Socket::PollIn, to);
		if (ret < 0) {
			return false;
		}

		sock->read(buf, sizeof(buf), true);

		hdr = (hci_event_hdr *) (buf + 1);
		ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
		len -= (1 + HCI_EVENT_HDR_SIZE);

		printf("Got HCI event, evt=0x%02x, plen=0x%02x\n",
				hdr->evt, hdr->plen);

		switch (hdr->evt) {

		/*
		   The Command Status event is used to indicate that the command described by
			the Command_Opcode parameter has been received, and that the Controller
			is currently performing the task for this command.
		 */
		case EVT_CMD_STATUS:
			cs = (evt_cmd_status *) ptr;

			if (cs->opcode != mOpcode)
				continue;

			if (mEvent != EVT_CMD_STATUS) {
				if (cs->status) {
					goto failed;
				}
				break;
			}

			memcpy(mReplyData, ptr, mReplyDataSize);
			goto done;

			/* Command was completed */
		case EVT_CMD_COMPLETE:
			cc = (evt_cmd_complete *) ptr;

			printf("Got EVT_CMD_COMPLETE, ncmd=0x%02x, opcode=0x%04x\n",
					cc->ncmd, cc->opcode);


			if (cc->opcode != mOpcode)
				continue;

			ptr += EVT_CMD_COMPLETE_SIZE;
			len -= EVT_CMD_COMPLETE_SIZE;

			memcpy(mReplyData, ptr, mReplyDataSize);
			goto done;

			/* LE meta event */
		case EVT_LE_META_EVENT:
			me = (evt_le_meta_event *) ptr;

			if (me->subevent != mEvent)
				continue;

			len -= 1;
			memcpy(mReplyData, ptr, mReplyDataSize);
			goto done;

		default:
			if (hdr->evt != mEvent)
				break;

			memcpy(mReplyData, ptr, mReplyDataSize);
			goto done;
		}
	}
failed:
	return false;
done:
	return true;
}
#endif

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
#if 0
	HCICmd cmd;

	cmd.type = HCI_COMMAND_PKT;
	cmd.opcode = mOpcode;
	cmd.dlen = mCmdDataSize;
	if (mCmdDataSize <= 256) {
		::memcpy(&cmd.data, mCmdData, mCmdDataSize);
	} else {
		return false;
	}

	int ret = sock->write(&cmd, 4 + mCmdDataSize);
#endif
	mCmd.type = HCI_COMMAND_PKT;
	int ret = sock->write(&mCmd, 4 + mCmd.dlen);

	if (ret < 0) {
		std::cout << "HciRequest::writeToSocket: Failed to send command\n";
		return false;
	}
	std::cout << "Wrote " << ret << " bytes\n";


	mReqWritten = true;
}

void HciRequest::complete(const uint8_t *reply, size_t len)
{
	if (mCompleteCallback)
		mCompleteCallback(reply, len);
}


}; // namespace BLE
