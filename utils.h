/*
 * utils.h
 *
 *  Created on: Apr 7, 2014
 *      Author: kuikka
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <stdint.h>

class hex
{
public:
	hex (uint8_t val) : mVal8(val), mType(8) {};
	hex (uint16_t val) : mVal16(val), mType(16) {};

protected:
	uint8_t mVal8;
	uint16_t mVal16;
	uint8_t mType;

	friend std::ostream& operator<<(std::ostream& os, const hex & h);
};

std::ostream& operator<<(std::ostream& os, const hex & h);


#endif /* UTILS_H_ */
