/*
 * BluetoothScanForm.h
 *
 *  Created on: Oct 14, 2013
 *      Author: kien
 */

#ifndef BLUETOOTHSCANFORM_H_
#define BLUETOOTHSCANFORM_H_

#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FNet.h>

class BluetoothScanForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
	, public Tizen::Net::Bluetooth::IBluetoothManagerEventListener
	, public Tizen::Net::Bluetooth::IBluetoothDeviceEventListener
{
public:
	BluetoothScanForm(void);
	virtual ~BluetoothScanForm(void);
	bool Initialize(void);

	virtual result OnInitializing(void);
	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);

	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
									const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
									const Tizen::Ui::Scenes::SceneId &nextSceneId) {}

	// from IBluetoothManagerEventListener
	virtual void OnBluetoothActivated(result r) {}
	virtual void OnBluetoothDeactivated(result r) {}

	// from IBluetoothDeviceEventListener
	virtual void OnBluetoothDiscoveryDone(bool isCompleted);
	virtual void OnBluetoothDiscoveryStarted(result r);
	virtual void OnBluetoothPaired(const Tizen::Net::Bluetooth::BluetoothDevice& pairedDevice){}
	virtual void OnBluetoothRemoteDeviceFoundN(Tizen::Net::Bluetooth::BluetoothDevice* pFoundDevice);
	virtual void OnBluetoothServiceListReceived(const Tizen::Net::Bluetooth::BluetoothDevice& targetDevice, unsigned long serviceList, result r){}
	virtual void OnBluetoothUnpaired(const Tizen::Net::Bluetooth::BluetoothDevice& unpairedDevice){}

private:
	static const int ID_OPTION_DONE = 100;

	Tizen::Net::Bluetooth::BluetoothManager __btManager;
	Tizen::Base::Collection::ArrayList __foundDeviceList;
	Tizen::Ui::Controls::ListView* __pDisplayDeviceListView;

	void ShowTimeoutMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text, unsigned long timeout);
};



#endif /* BLUETOOTHSCANFORM_H_ */
