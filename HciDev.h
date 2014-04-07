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
#include <set>

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
		ALL,
		WHITELIST_ONLY,
	} ScanningFilterPolicy;

public:
	HciDev(int devId, MainLoop *loop);
	virtual ~HciDev();
	bool devInfo();
	HciSocket* getSocket();
	MainLoop* getMainLoop() { return mLoop; };

public:
	bool leSetScanParameters(ScanType scanType,
			uint16_t scanInterval,
			uint16_t scanWindow,
			BLEAddress::AddressType ownAddressType,
			ScanningFilterPolicy filterPolicy);

	bool leScanEnable(bool enable, bool filterDuplicates);

	bool leConnect(BLEDevice *dev = NULL, bool useWhiteList = false);
	bool leDisconnect(BLEDevice *bleDev);
	bool leCancelConnection();
	bool leAddToWhitelist();
	bool leReadBufferSize();
	bool leReadWhiteListSize();
	bool leClearWhiteList();
	bool leAddToWhitelist(const BLEAddress &addr);
	bool leConnectViaWhiteList(BLEDevice *dev);

public:
	void setScanListener(BLEScanListener *listener);
	bool open();
	bool up();
	bool down();
	void addDevice(BLEDevice *dev);
	void removeDevice(BLEDevice *dev);
	bool disconnect(BLEDevice *bleDev = NULL);



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

	void onCommandCompleted(uint16_t opCode, const uint8_t *data, size_t dataLen);
	void onCommandStatus(uint16_t opCode, uint8_t status);
	void onDisconnectionEvent(uint8_t status, uint16_t handle, uint8_t reason);
	void onLEMetaEvent(uint8_t subEvent, const uint8_t *data, size_t datalen);

	BLEDevice* findDeviceByHandle(uint16_t handle);

public:
	BLEDevice* findDeviceByAddress(const BLEAddress &addr);




protected:
	HciSocket *mSocket;
	int mDevId;
	BLEScanListener *mScanListener;
	MainLoop *mLoop;
	typedef std::queue<HciRequest*> RequestQueue;
	RequestQueue mRequestQueue;
	typedef std::set<BLEDevice*> DeviceMap;
	DeviceMap mDevices;
	HciRequest *mCurrentRequest;
	bool mScanning;
	bool mConnecting;
	unsigned mWhiteListSize;

	friend class HciSocket;
};

};
#endif /* HCIDEV_H_ */
