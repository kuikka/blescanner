/*
 * utils.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: kuikka
 */

#include <iomanip>
#include "utils.h"

std::ostream& operator<<(std::ostream& os, const hex & h)
{
	std::ios_base::fmtflags flags = os.flags();
	os << "0x";
	os << std::hex << std::setfill('0');
	if (h.mType == 8) {
		os << std::setw(2);
		os << (unsigned)h.mVal8;
	} else if (h.mType == 16) {
		os << std::setw(4);
		os << (unsigned)h.mVal16;
	}
	os << std::setiosflags(flags);
	return os;
}
