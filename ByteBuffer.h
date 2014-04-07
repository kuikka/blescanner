/*
 * ByteBuffer.h
 *
 *  Created on: Apr 6, 2014
 *      Author: kuikka
 */

#ifndef BYTEBUFFER_H_
#define BYTEBUFFER_H_

#include <stdint.h>
#include <unistd.h>
#include <vector>
#include "UUID.h"

class Socket;

class ByteBuffer {
public:
	ByteBuffer(size_t capacity);
	ByteBuffer(uint8_t *buf, size_t capacity);
	virtual ~ByteBuffer();
	void put(uint8_t data);
	void put(uint16_t data);
	void put(const UUID & uuid);
	uint8_t get8();
	uint16_t get16();

	bool write(Socket *sock);

protected:
	uint8_t *mBuffer;
	size_t mCapacity;
	size_t mUsed;
	bool mAllocated;
};

#endif /* BYTEBUFFER_H_ */
