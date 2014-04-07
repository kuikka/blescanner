/*
 * UUID.h
 *
 *  Created on: Apr 6, 2014
 *      Author: kuikka
 */

#ifndef UUID_H_
#define UUID_H_

#include <stdint.h>

class UUID {

public:
	typedef enum {
		Uuid16,
		Uuid128,
	} UUIDType;
	UUID();
	UUID(uint16_t uuid);
	virtual ~UUID();

	UUIDType getType() const;
	uint16_t get16() const;

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
};

#endif /* UUID_H_ */
