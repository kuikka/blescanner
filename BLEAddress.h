/*
 * BLEAddress.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef BLEADDRESS_H_
#define BLEADDRESS_H_

#include <stdint.h>
#include <string.h>

namespace BLE {

class BLEAddress
{
public:
	typedef enum {
		PUBLIC,
		PRIVATE,
	} AddressType;

public:
	BLEAddress(AddressType type = PUBLIC, const uint8_t *b = NULL) : type(type) { ::memcpy(this->b, b, 6); };

public:
	uint8_t b[6];
	AddressType type;
};

} /* namespace BLE */

#endif /* BLEADDRESS_H_ */
