/*
 * BLEAddress.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef BLEADDRESS_H_
#define BLEADDRESS_H_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdint.h>
#include <string.h>
#include <functional>
#include <stdlib.h>

#include "hci.h"


namespace BLE {

class BLEAddress
{
public:
	typedef enum {
		PUBLIC,
		RANDOM,
	} AddressType;

public:
	// little endian
	bdaddr_t address;
	AddressType type;

public:
	BLEAddress(const uint8_t *b = NULL, AddressType type = PUBLIC) : type(type)
{
		if(b) {
			::memcpy(this->address.b, b, 6);
		}
};
	BLEAddress(const uint8_t  (&b) [6], AddressType type = PUBLIC) : type(type)
	{
		for (unsigned i = 0; i < 6; i++)
			this->address.b[i] = b[i];
	}
	BLEAddress(const bdaddr_t & addr, AddressType type = PUBLIC) : type(type), address(addr)
	{};

	BLEAddress(const char* str, AddressType type = PUBLIC) : type(type)
	{
		for (int i = 5; i >= 0 ; i--, str += 3)
			address.b[i] = strtoul(str, NULL, 16);
	}



public:
	bool operator==(const BLEAddress &other)
	{
		if (::memcmp(&address, &other.address, 6) == 0 &&
				type == other.type)
			return true;
		return false;
	};
};

inline bool operator<(const BLEAddress &l, const BLEAddress &r)
{
	int c = ::memcmp(l.address.b, r.address.b, sizeof(l.address.b));
	if (c < 0) {
		return true;
	}
	else if (c > 0) {
		return false;
	}
	else if (c == 0) {
		return l.type < r.type;
	}
	return false; // not reached
}

struct  BLEAddressLessThan : public std::less<BLEAddress>
{
	bool operator()(const BLEAddress &l, const BLEAddress &r) const
	{
		return l < r;
	};
};


inline std::ostream& operator<<(std::ostream& os, const BLEAddress &addr)
{
	std::stringstream ss;
	ss << std::hex << std::setw(2) << std::setfill('0');
	for (int i = 5; i >= 0; i--) {
		ss << (unsigned)addr.address.b[i];
		ss << ":";
	}
	os << ss.str();
	os << " ";
	os << (addr.type == BLE::BLEAddress::PUBLIC ? "Public" : "Random");
	return os;
}

} /* namespace BLE */

#endif /* BLEADDRESS_H_ */
