/*
 * BLEGATT.cpp
 *
 *  Created on: Mar 27, 2014
 *      Author: kuikka
 */

#include <stdio.h>
#include "BLEGATT.h"

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

BLEGATT::BLEGATT(BLEDevice *dev) : mDev(dev), mL2CAP(dev)
{
	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);
}

BLEGATT::~BLEGATT()
{
}

bool BLEGATT::connect()
{
	printf("%s this=%p\n", __PRETTY_FUNCTION__, this);
	mL2CAP.connect();
}

bool BLEGATT::discoverAll()
{
	uint8_t buf[7];
	buf[0] = ATT_OP_READ_BY_GROUP_REQ;
	buf[1] = 0x01;
	buf[2] = 0x00;
	buf[3] = 0xFF;
	buf[4] = 0xFF;
	buf[5] = 0x00;
	buf[6] = 0x28;

	mL2CAP.write(buf, sizeof(buf));
}

} /* namespace BLE */
