/*
 * BLEScanListener.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef BLESCANLISTENER_H_
#define BLESCANLISTENER_H_

#include "BLEAddress.h"

namespace BLE {

class HciDev;

class BLEScanListener
{
public:
	typedef enum {
		ConnectableUndirected,
		ConnectableDirected,
		ScannableUndirected,
		NotConnectableUndirected,
		ScanResponse,
	} EventType;

public:
	virtual ~BLEScanListener() {};
	virtual void onAdvertisingReport(HciDev &hciDev,
			EventType type,
			BLE::BLEAddress from,
			int8_t rssi,
			const uint8_t *data,
			size_t datalen) = 0;
};

} // namespaec BLE

#endif /* BLESCANLISTENER_H_ */
