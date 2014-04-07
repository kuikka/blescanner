/*
 * UUID.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: kuikka
 */

#include "UUID.h"

UUID::UUID() {
	// TODO Auto-generated constructor stub

}

UUID::~UUID() {
	// TODO Auto-generated destructor stub
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
