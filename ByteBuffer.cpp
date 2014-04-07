/*
 * ByteBuffer.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: kuikka
 */

#include "ByteBuffer.h"
#include "Socket.h"

ByteBuffer::ByteBuffer(size_t capacity) : mCapacity(capacity), mUsed(0), mAllocated(true)
{
	mBuffer = new uint8_t[mCapacity];
}

ByteBuffer::ByteBuffer(uint8_t *buf, size_t capacity) : mCapacity(capacity),
		mAllocated(false), mUsed(0), mBuffer(buf)
{

}

ByteBuffer::~ByteBuffer()
{
	if (mAllocated)
		delete mBuffer;
}

void ByteBuffer::put(uint16_t data)
{
	mBuffer[mUsed++] = data & 0x00FF;
	mBuffer[mUsed++] = data >> 8;
}

void ByteBuffer::put(uint8_t data)
{
	mBuffer[mUsed++] = data;
}

bool ByteBuffer::write(Socket *sock)
{
	return sock->write(mBuffer, mUsed);
}

void ByteBuffer::put(const UUID & uuid)
{
	if (uuid.getType() == UUID::Uuid16) {
		put(uuid.get16());
	}
}

uint8_t ByteBuffer::get8()
{
	return mBuffer[mUsed++];
}

uint16_t ByteBuffer::get16()
{
	uint16_t val;

	val = mBuffer[mUsed++];
	val = val |  (mBuffer[mUsed++] << 8);

	return val;
}
