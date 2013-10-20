//
// Tizen Native SDK
// Copyright (c) 2012-2013 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.1 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef _MAIN_FORM_H_
#define _MAIN_FORM_H_

#include <unique_ptr.h>
#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FGraphics.h>
#include <FNet.h>

namespace ButtonActionType
{
    enum Enum { BT_ON_OFF,
    			WIFI_ON_OFF,
                SCAN_CONNECT,
                CREATE_AUTO_GROUP,
                LEAVE_GROUP,
                START_CHAT,
                COUNT};
}

class MainForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::ITextEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
	, public Tizen::Net::Wifi::IWifiDirectDeviceListener
	, public Tizen::Net::Wifi::IWifiManagerEventListener
	, public Tizen::Net::Bluetooth::BluetoothDevice
	//, public Tizen::Net::Bluetooth::BluetoothSppInitiator
	, public Tizen::Net::Bluetooth::IBluetoothDeviceEventListener
	, public Tizen::Net::Bluetooth::IBluetoothManagerEventListener
	, public Tizen::Net::Bluetooth::IBluetoothSppInitiatorEventListener
{
	enum CurrentState { STATE_DEACTIVATED,
	                    STATE_ACTIVATED,
	                    STATE_AUTO_GROUP_OWNER,
	                    STATE_GROUP_OWNER,
	                    STATE_GROUP_CLIENT };
	enum ControlType { CONTROL_EDIT_FILED,
	                   CONTROL_BUTTON,
	                   CONTROL_CHECK_BUTTON_STYLE_RADIO};

public:
	MainForm(void);
	virtual ~MainForm(void);
	bool Initialize(void);

	result Activate(void);
	result Scan(void);
	result Connect(const Tizen::Net::Wifi::WifiDirectDeviceInfo& remoteDeviceInfo);
	result Disconnect(const Tizen::Net::Wifi::WifiDirectDeviceInfo& peerDeviceInfo);

	// Form
	virtual result OnInitializing(void);

	// IActionEventListener
	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);

	// ITextEventListener
	virtual void OnTextValueChanged(const Tizen::Ui::Control& source);
	virtual void OnTextValueChangeCanceled(const Tizen::Ui::Control& source){}

	// IFormBackEventListener
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	// ISceneEventListener
	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
	                                const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
	                                const Tizen::Ui::Scenes::SceneId &nextSceneId);

	// IWifiDirectDeviceListener
	virtual void OnWifiDirectDeviceActivated(Tizen::Net::Wifi::WifiDirectDeviceId localDeviceId, result r);
	virtual void OnWifiDirectDeviceDeactivated(Tizen::Net::Wifi::WifiDirectDeviceId localDeviceId, result r);
	virtual void OnWifiDirectConnected(Tizen::Net::Wifi::WifiDirectDeviceId localDeviceId,
									   const Tizen::Net::Wifi::WifiDirectDeviceInfo& remoteDeviceInfo, result r);
	virtual void OnWifiDirectDisconnected(Tizen::Net::Wifi::WifiDirectDeviceId localDeviceId, const Tizen::Base::String& peerMacAddress, result r);
	virtual void OnWifiDirectAutonomousGroupCreated(Tizen::Net::Wifi::WifiDirectDeviceId localDeviceId, result r);
	virtual void OnWifiDirectGroupLeft(Tizen::Net::Wifi::WifiDirectDeviceId localDeviceId, result r);
	virtual void OnWifiDirectScanCompletedN(Tizen::Net::Wifi::WifiDirectDeviceId localDeviceId,
											Tizen::Base::Collection::IList *pWifiDirectDeviceInfoList, result r);

	virtual void OnWifiActivated(result r){}
	virtual void OnWifiDeactivated(result r);
	virtual void OnWifiConnected(const Tizen::Base::String& ssid, result r){}
	virtual void OnWifiDisconnected(){}
	virtual void OnWifiRssiChanged(long rssi){}
	virtual void OnWifiScanCompletedN(const Tizen::Base::Collection::IList* pWifiBssInfoList, result r){}

	// IBluetoothDeviceEventListener
	virtual void  OnBluetoothDiscoveryDone (bool isCompleted){}
	virtual void  OnBluetoothDiscoveryStarted (result r){}
	virtual void  OnBluetoothPaired (const BluetoothDevice &pairedDevice){}
	virtual void  OnBluetoothPairingFailed (result r) {}
	virtual void  OnBluetoothRemoteDeviceFoundN (BluetoothDevice *pFoundDevice){}
	virtual void  OnBluetoothServiceListReceived (const BluetoothDevice &targetDevice, unsigned long serviceList, result r){}
	virtual void  OnBluetoothUnpaired (const BluetoothDevice &unpairedDevice){}

#if 1
	// IBluetoothSppInitiatorEventListener
	virtual void  OnSppConnectionResponded (result r) {}
	virtual void  OnSppDataReceived (Tizen::Base::ByteBuffer &buffer) {}
	virtual void  OnSppDisconnected (result r) {}
#endif

	// IBluetoothManagerEventListener
	virtual void  OnBluetoothActivated (result r);
	virtual void  OnBluetoothDeactivated (result r);

private:
	void ShowMainForm(void);
	void ShowDeviceInfoForm(void);
	void ShowGroupConfigurationForm(void);
	void ShowChatForm(void);
	void ShowRobotChatForm(void);
	void ShowChatPortConfigurationForm(void);
	void ShowWpsConfigurationModeForm(void);
	void ShowPeerDeviceListForm(void);
	void ShowBluetoothScanForm(void);
	void ShowBluetoothDebugForm(void);

	void ShowMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text);

	void ShowScanPopup(void);
	void HideScanPopup(void);

	void ShowConnectPopup(void);
	void HideConnectPopup(void);

	void ShowWaitPopup(const Tizen::Base::String& text);
	void HideWaitPopup(void);

	void UpdateControl(void);
	bool UpdateCurrentState(void);
	bool UpdateLocalDeviceName(const Tizen::Base::String& deviceName = L"");

	bool AddMainControl(ControlType type, const Tizen::Base::String& text, int firstActionId = -1, int secondActionId = -1);
	bool AddPopup(void);

private:
	// Button constant
    static const int ID_BUTTON_ACTIVATE = 100;
    static const int ID_BUTTON_DEACTIVATE = 101;
    static const int ID_BUTTON_SCAN = 102;
    static const int ID_BUTTON_CONNECT = 104;
    static const int ID_BUTTON_CREATE_AUTO_GROUP = 105;
    static const int ID_BUTTON_LEAVE_GROUP = 106;
    static const int ID_BUTTON_START_CHAT = 107;
    static const int ID_BUTTON_DISCONNECT = 108;

    static const int ID_BUTTON_CANCEL_SCAN = 109;
    static const int ID_BUTTON_CANCEL_CONNECT = 110;

    static const int ID_BUTTON_BLUETOOTH_TOGGLED = 111;

    // Footer constant
    static const int ID_FOOTER_DEVICE_INFO = 200;
    static const int ID_FOOTER_GROUP_CONFIG = 201;
    static const int ID_FOOTER_WPS_CONFIG = 202;
    static const int ID_FOOTER_CHAT_PORT_CONFIG = 203;
    static const int ID_FOOTER_PEER_DEVICE_LIST = 204;

    static const int DEFAULT_CHAT_PORT = 9000;

    Tizen::Base::Collection::ArrayList __controlList;
    Tizen::Ui::Controls::EditField* __pEditFieldDeviceName;

    Tizen::Ui::Controls::Popup* __pPopupWait;
    Tizen::Ui::Controls::Popup* __pPopupScan;
    Tizen::Ui::Controls::Popup* __pPopupConnect;
    Tizen::Ui::Controls::Label* __pLabel;


    std::unique_ptr<Tizen::Net::Wifi::WifiDirectDevice> __pWifiDirectLocalDevice;
    Tizen::Base::String __chatPortNumber;

	CurrentState __currentState;
	bool __isWiifDirectSupported;
	bool __isWifiDeactivating;
	Tizen::Net::Wifi::WifiManager __wifiManager;

	bool __isBtActivating;
	bool __isBtDeactivating;
	Tizen::Net::Bluetooth::BluetoothManager __btManager;
#if 0
	bool __isBtKBTReady;
	Tizen::Net::Bluetooth::BluetoothSppInitiator __sppInitiator;
#endif
};

#endif  //_MAIN_FORM_H_
