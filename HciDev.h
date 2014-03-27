/*
 * HciDev.h
 *
 *  Created on: Mar 16, 2014
 *      Author: kuikka
 */

#ifndef HCIDEV_H_
#define HCIDEV_H_

#include <stdint.h>
#include "HciSocket.h"
#include "BLEScanListener.h"
#include "HciRequest.h"
#include <queue>

class MainLoop;

namespace BLE {

class BLEScanListener;
class BLEDevice;

class HciDev {
public:
	typedef enum {
		PASSIVE,
		ACTIVE,
	} ScanType;

	typedef enum {
		PUBLIC,
		PRIVATE,
	} AddressType;

	typedef enum {
		ALL,
		WHITELIST_ONLY,
	} ScanningFilterPolicy;

public:
	HciDev(int devId, MainLoop *loop);
	virtual ~HciDev();
	bool devInfo();
	HciSocket* getSocket();

public:
	bool leSetScanParameters(ScanType scanType,
			uint16_t scanInterval,
			uint16_t scanWindow,
			AddressType ownAddressType,
			ScanningFilterPolicy filterPolicy);

	bool leScanEnable(bool enable, bool filterDuplicates);

	void setScanListener(BLEScanListener *listener);
	bool open();
	bool connect(BLEDevice *dev);



protected: // Used by HciSocket
	bool wantToWrite();
	bool wantToRead();
	bool onPollIn();
	bool onPollOut();
	bool onPollError();

protected:
	bool submit(HciRequest *req);
	bool readFromSocket();
	void completeCurrentRequest(const uint8_t *ptr, size_t len);
	void handleAdvertisingReport(const uint8_t *data, size_t len);



protected:
	HciSocket *mSocket;
	int mDevId;
	BLEScanListener *mScanListener;
	MainLoop *mLoop;
	typedef std::queue<HciRequest*> RequestQueue;
	RequestQueue mRequestQueue;
	HciRequest *mCurrentRequest;

	friend class HciSocket;
};

};
#endif /* HCIDEV_H_ */
