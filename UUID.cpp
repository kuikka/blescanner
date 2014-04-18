/*
 * UUID.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: kuikka
 */

#include <string.h>
#include <sstream>
#include "UUID.h"

UUID::UUID()
{
}

UUID::UUID(const uint8_t *data, size_t length)
{
	if (length == 2) {
		mType = Uuid16;
		::memcpy(&mData.uuid16, data, 2);
	}
	if (length == 16) {
		mType = Uuid128;
		::memcpy(&mData.uuid128, data, 16);
	}
}

UUID::~UUID()
{
}

UUID::UUID(uint16_t uuid) : mType(Uuid16)
{
	mData.uuid = uuid;
}

UUID::UUIDType UUID::getType() const
{
	return mType;
}

uint16_t UUID::get16() const
{
	return mData.uuid;
}

std::ostream& operator<<(std::ostream &os, const UUID &uuid)
{
	if (uuid.getType() == UUID::Uuid16) {
		std::stringstream ss;
		uint16_t val = uuid.mData.uuid16[0];
		val |= uuid.mData.uuid16[1] << 8;

		ss << "0x";
		ss << std::hex << std::setw(4) << std::setfill('0');
		ss << val;
		os << ss.str();
	}
	return os;
}
