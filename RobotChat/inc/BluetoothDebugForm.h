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


namespace ButtonActionType2
{
    enum Enum { CONNECT,
    			DISCONNECT,
    			MOVE_HEAD,
    			PLAY_MOTION_1,
    			PLAY_MOTION_2,
    			PLAY_MOTION_3,
    			PLAY_MOTION_4,
    			PLAY_MOTION_5,
    			PLAY_MOTION_6,
    			PLAY_MOTION_7,
    			PLAY_MOTION_8,
               };
}

class BluetoothDebugForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
	, public Tizen::Net::Bluetooth::IBluetoothManagerEventListener
	, public Tizen::Net::Bluetooth::IBluetoothSppInitiatorEventListener
	, public Tizen::Net::Bluetooth::IBluetoothSppAcceptorEventListener
{
		enum ControlType { CONTROL_EDIT_FILED,
		                   CONTROL_BUTTON,
		                   CONTROL_CHECK_BUTTON_STYLE_RADIO};
		enum RobotCurrentState { ROBOT_STATE_IDLE,
		                    ROBOT_STATE_IN_MOTION,
		                    ROBOT_STATE_UNKNOWN };

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

	// IBluetoothSppInitiatorEventListener
	virtual void OnSppConnectionResponded(result r);
	virtual void OnSppDataReceived(Tizen::Base::ByteBuffer &buffer);
	virtual void OnSppDisconnected(result r);

	// IBluetoothSppInitiatorEventListener
	virtual void OnSppConnectionRequested(const Tizen::Net::Bluetooth::BluetoothDevice &device);


private:
	// Button constant
    static const int ID_BUTTON_CONNECT = 1000;
    static const int ID_BUTTON_DISCONNECT = 1001;
    static const int ID_BUTTON_ACCEPTOR = 1002;
    static const int ID_BUTTON_PING = 1010;
    static const int ID_BUTTON_MOVE_HEAD = 1100;
    static const int ID_BUTTON_PLAYMOTION_1 = 2001;
    static const int ID_BUTTON_PLAYMOTION_2 = 2002;
    static const int ID_BUTTON_PLAYMOTION_3 = 2003;
    static const int ID_BUTTON_PLAYMOTION_4 = 2004;
    static const int ID_BUTTON_PLAYMOTION_5 = 2005;
    static const int ID_BUTTON_PLAYMOTION_6 = 2006;
    static const int ID_BUTTON_PLAYMOTION_7 = 2007;
    static const int ID_BUTTON_PLAYMOTION_8 = 2008;
	static const int ID_OPTION_DONE = 100;
    Tizen::Base::Collection::ArrayList __controlList;

	bool __isBtKBTReady;		// true if bluetooth is activated
	bool __isSppConstructed;	// ture if spp stack ready
	bool __isBtKBTConnectionResponded;		// true if connection established
	Tizen::Net::Bluetooth::BluetoothManager __btManager;
	Tizen::Net::Bluetooth::BluetoothSppInitiator __sppInitiator;
	Tizen::Net::Bluetooth::BluetoothSppAcceptor __sppAcceptor;

	bool AddMainControl(ControlType type, const Tizen::Base::String& text, int firstActionId = -1, int secondActionId = -1);
	void ShowTimeoutMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text, unsigned long timeout);

	int robotState;		// unused
	byte RCB4_calc_checksum(byte *cmd, int len);
	result Connect_KBT1();
	result Disconnect_KBT1();
	result Initialize_SppAcceptor();
	result KBT1_Ping();
	result KBT1_Move_Head();
	result KBT1_PlayMotion(int index);	/* 0 to n */
};



#endif /* BLUETOOTHDEBUGFORM_H */
