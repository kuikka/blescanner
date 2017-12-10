/*
 * BLEGATT.cpp
 *
 *  Created on: Mar 27, 2014
 *      Author: kuikka
 */

#include <stdio.h>
#include "BLEDevice.h"
#include "BLEGATT.h"
#include "ByteBuffer.h"
#include "utils.h"

#define ATT_OP_ERROR			0x01
#define ATT_OP_MTU_REQ			0x02
#define ATT_OP_MTU_RESP			0x03
#define ATT_OP_FIND_INFO_REQ		0x04
#define ATT_OP_FIND_INFO_RESP		0x05
#define ATT_OP_FIND_BY_TYPE_REQ		0x06
#define ATT_OP_FIND_BY_TYPE_RESP	0x07
#define ATT_OP_READ_BY_TYPE_REQ		0x08
#define ATT_OP_READ_BY_TYPE_RESP	0x09
#define ATT_OP_READ_REQ			0x0A
#define ATT_OP_READ_RESP		0x0B
#define ATT_OP_READ_BLOB_REQ		0x0C
#define ATT_OP_READ_BLOB_RESP		0x0D
#define ATT_OP_READ_MULTI_REQ		0x0E
#define ATT_OP_READ_MULTI_RESP		0x0F
#define ATT_OP_READ_BY_GROUP_REQ	0x10
#define ATT_OP_READ_BY_GROUP_RESP	0x11
#define ATT_OP_WRITE_REQ		0x12
#define ATT_OP_WRITE_RESP		0x13
#define ATT_OP_WRITE_CMD		0x52
#define ATT_OP_PREP_WRITE_REQ		0x16
#define ATT_OP_PREP_WRITE_RESP		0x17
#define ATT_OP_EXEC_WRITE_REQ		0x18
#define ATT_OP_EXEC_WRITE_RESP		0x19
#define ATT_OP_HANDLE_NOTIFY		0x1B
#define ATT_OP_HANDLE_IND		0x1D
#define ATT_OP_HANDLE_CNF		0x1E
#define ATT_OP_SIGNED_WRITE_CMD		0xD2


namespace BLE {

BLEGATT::BLEGATT(BLEDevice *dev) : mDev(dev), mL2CAP(this, dev->getHciDev()->getMainLoop()), mDiscovering(false)
{
//	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);
}

BLEGATT::~BLEGATT()
{
}

bool BLEGATT::connect()
{
//	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);
	return mL2CAP.connect(mDev->getAddress());
}

bool BLEGATT::disconnect()
{
//	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);
	return mL2CAP.close();
}

bool BLEGATT::discoverAll()
{
	mDiscovering = true;

	return opReadByGroupTypeReq(0x0001, 0xFFFF, UUID(0x2800));
}

bool BLEGATT::opReadByGroupTypeReq(uint16_t startHandle, uint16_t endHandle, const UUID & uuid)
{
	ByteBuffer bb(21); // Max
	bb.put((uint8_t)ATT_OP_READ_BY_GROUP_REQ);
	bb.put(startHandle);
	bb.put(endHandle);
	bb.put(uuid);
	mReqType = uuid;

	return bb.write(&mL2CAP);
}

#define GATT_MTU 23

void BLEGATT::readFromSocket()
{
	uint8_t buf[GATT_MTU];
	uint16_t end_handle;

	int ret = mL2CAP.read(buf, sizeof(buf), true);
	if (ret <= 0)
			return ;

	ByteBuffer bb(buf, sizeof(buf));

	std::cout << "GATT: Got " << ret << " bytes\n";

	uint8_t opcode = bb.get8();

	switch (opcode) {
		case ATT_OP_READ_BY_GROUP_RESP:
		{
			std::cout << "Read by group response\n";
			uint8_t length = bb.get8();
			std::cout << "  Got attributes, length=" << (int)length << "\n";
			if (length == 6) {
				unsigned nr_data = ((ret - 2) / length);
				while (nr_data--) {
					ATT_entry att;
					att.handle = bb.get16();
					att.last_handle_in_group = bb.get16();
					att.value.push_back(bb.get8());
					att.value.push_back(bb.get8());
					att.type = mReqType;
					end_handle = att.last_handle_in_group;

					printf("    Attribute handle 0x%04x\n", att.handle);
					printf("    End handle 0x%04x\n", att.last_handle_in_group);
					printf("    Value 0x%02x%02x\n", att.value[1], att.value[0]);
					std::cout << "    Type=" << att.type << "\n";
				}
			}
			else if (length == 20) {
				unsigned nr_data = ((ret - 2) / length);
				while (nr_data--) {
					ATT_entry att;
					att.handle = bb.get16();
					att.last_handle_in_group = bb.get16();
					att.type = mReqType;
					end_handle = att.last_handle_in_group;

					for (unsigned i = 0; i < 16; i++) {
						att.value.push_back(bb.get8());
					}
				}
			}

			if (end_handle != 0xFFFF) {
				opReadByGroupTypeReq(end_handle + 1, 0xFFFF, mReqType);
			}
		}
		break;
		case ATT_OP_ERROR:
		{
			std::cout << "GATT: Error, opcode=" << hex(bb.get8())
					<< " handle=" << hex(bb.get16())
					<< " error=" << hex(bb.get8())
					<< std::endl;
		}
		break;

		default:
		{
			std::cout << "GATT: Got opcode " << hex(opcode) << std::endl;
		}
		break;

	}

}

bool BLEGATT::onPollIn()
{
	readFromSocket();
	return true;
}

bool BLEGATT::onPollOut()
{
	return false;
}

bool BLEGATT::wantToWrite()
{
	return false;
}

} /* namespace BLE */
