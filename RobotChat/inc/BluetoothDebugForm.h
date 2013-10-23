/*
 * BluetoothDebugForm.h
 *
 *  Created on: Oct 19, 2013
 *      Author: Mori
 */

#ifndef BLUETOOTHDEBUGFORM_H_
#define BLUETOOTHDEBUGFORM_H_

#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FNet.h>

class BluetoothDebugForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
	, public Tizen::Net::Bluetooth::IBluetoothManagerEventListener
	, public Tizen::Net::Bluetooth::IBluetoothDeviceEventListener
	, public Tizen::Net::Bluetooth::IBluetoothSppInitiatorEventListener
	, public Tizen::Net::Bluetooth::IBluetoothSppAcceptorEventListener
{
		enum ControlType { CONTROL_EDIT_FILED,
		                   CONTROL_BUTTON,
		                   CONTROL_CHECK_BUTTON_STYLE_RADIO};
public:
	BluetoothDebugForm(void);
	virtual ~BluetoothDebugForm(void);
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

	// IBluetoothSppInitiatorEventListener
	virtual void OnSppConnectionResponded(result r);
	virtual void OnSppDataReceived(Tizen::Base::ByteBuffer &buffer);
	virtual void OnSppDisconnected(result r);

	// IBluetoothSppInitiatorEventListener
	virtual void OnSppConnectionRequested(const Tizen::Net::Bluetooth::BluetoothDevice &device);

private:
	// Button constant
    static const int ID_BUTTON_CONNECT = 1000;
    static const int ID_BUTTON_ACCEPTOR = 1001;
    static const int ID_BUTTON_PING = 1002;
    static const int ID_BUTTON_GOFORWARD = 1008;
	static const int ID_OPTION_DONE = 100;
    Tizen::Base::Collection::ArrayList __controlList;

	bool __isBtKBTReady;
	bool __isBtKBTConnectionResponded;
	Tizen::Net::Bluetooth::BluetoothManager __btManager;
	Tizen::Net::Bluetooth::BluetoothSppInitiator __sppInitiator;
	Tizen::Net::Bluetooth::BluetoothSppAcceptor __sppAcceptor;

	Tizen::Base::Collection::ArrayList __foundDeviceList;
	Tizen::Ui::Controls::ListView* __pDisplayDeviceListView;

	bool AddMainControl(ControlType type, const Tizen::Base::String& text, int firstActionId = -1, int secondActionId = -1);
	void ShowTimeoutMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text, unsigned long timeout);

	result Connect_KBT1();
	result Initialize_SppAcceptor();
	result KBT1_Ping();
	result KBT1_GoForward();
};



#endif /* BLUETOOTHDEBUGFORM_H */
