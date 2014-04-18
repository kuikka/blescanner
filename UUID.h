/*
 * UUID.h
 *
 *  Created on: Apr 6, 2014
 *      Author: kuikka
 */

#ifndef UUID_H_
#define UUID_H_

#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>

class UUID {

public:
	typedef enum {
		Uuid16,
		Uuid128,
	} UUIDType;
	UUID();
	UUID(uint16_t uuid);
	UUID(const uint8_t *data, size_t length);
	virtual ~UUID();

	UUIDType getType() const;
	uint16_t get16() const;

public:


protected:
	UUIDType mType;
	typedef struct {
		union {
			uint16_t uuid;
			uint8_t uuid16[2];
			uint8_t uuid128[16];
		};
	} UuidData;
	UuidData mData;

	friend std::ostream& operator<<(std::ostream &os, const UUID &uuid);
};

std::ostream& operator<<(std::ostream &os, const UUID &uuid);

#endif /* UUID_H_ */
