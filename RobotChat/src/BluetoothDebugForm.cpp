/*
 * BluetoothDebugForm.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: Mori
 */
#include "BluetoothDebugForm.h"
#include "FormFactory.h"
#include "MainForm.h"

using namespace std;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Net::Bluetooth;


BluetoothDebugForm::BluetoothDebugForm()
	: __isSppConstructed(false)
	, __isBtKBTReady(false)
	, __isBtKBTConnectionResponded(false)
	, robotState(ROBOT_STATE_UNKNOWN) {
}

BluetoothDebugForm::~BluetoothDebugForm(void)
{
	/*
	if ( __isBtKBTReady ) {
		//Disconnect from the SPP acceptor:
		__sppInitiator.Disconnect();
		__isBtKBTReady = false;
	}
	__btManager.Deactivate();
	*/
}

bool
BluetoothDebugForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL | FORM_STYLE_HEADER | FORM_STYLE_FOOTER);

	SetName(FORM_BLUETOOTH_DEBUG);

	Header* pHeader = GetHeader();
	if (pHeader != null)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
		pHeader->SetTitleText(L"Bluetooth Debug");
	}

	Footer* pFooter = GetFooter();
	if (pFooter != null)
	{
		pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);

		FooterItem footerItemDone;
		footerItemDone.Construct(ID_OPTION_DONE);
		footerItemDone.SetText("Done");
		pFooter->AddItem(footerItemDone);
		pFooter->AddActionEventListener(*this);
	}

	AddMainControl(CONTROL_BUTTON, L"Connect", ID_BUTTON_CONNECT);
	AddMainControl(CONTROL_BUTTON, L"Disconnect", ID_BUTTON_DISCONNECT);
	//AddMainControl(CONTROL_BUTTON, L"Acceptor", ID_BUTTON_ACCEPTOR);
	//AddMainControl(CONTROL_BUTTON, L"Ping", ID_BUTTON_PING);
	AddMainControl(CONTROL_BUTTON, L"Move Head", ID_BUTTON_MOVE_HEAD);
	AddMainControl(CONTROL_BUTTON, L"Play Motion 1", ID_BUTTON_PLAYMOTION_1);
	AddMainControl(CONTROL_BUTTON, L"Play Motion 2", ID_BUTTON_PLAYMOTION_2);
	AddMainControl(CONTROL_BUTTON, L"Play Motion 3", ID_BUTTON_PLAYMOTION_3);
	AddMainControl(CONTROL_BUTTON, L"Play Motion 4", ID_BUTTON_PLAYMOTION_4);
	AddMainControl(CONTROL_BUTTON, L"Play Motion 5", ID_BUTTON_PLAYMOTION_5);
	AddMainControl(CONTROL_BUTTON, L"Play Motion 6", ID_BUTTON_PLAYMOTION_6);
	AddMainControl(CONTROL_BUTTON, L"Play Motion 7", ID_BUTTON_PLAYMOTION_7);
	AddMainControl(CONTROL_BUTTON, L"Play Motion 8", ID_BUTTON_PLAYMOTION_8);
	//AddPopup();

	SetFormBackEventListener(this);

	// Bluetooth initialize section
	if (IsFailed(__btManager.Construct(*this)))
	{
		AppLog("BluetoothManager::Construct() [Error].");
		return false;
	}

	if (!__btManager.IsActivated())
	{
		__btManager.Activate();
		//ShowWaitPopup(L"Activating bluetooth...");
		//ShowTimeoutMessageBox(L"Bluetooth Scan", L"Please Activate Bluetooth", 3000);
		//return false;
	}

	//__btManager.SetBluetoothDeviceListener(this);

#if 0
	// list Bluetooth info
	{
		String localName = __btManager.GetLocalDeviceName();
		String localAddress = __btManager.GetLocalDeviceAddress(); // For example: "00:3D:47:EF:8A:03"
		AppLog("Bluetooth: localName=%S.\n", localName.GetPointer());
		AppLog("Bluetooth: localAddress=%S.\n", localAddress.GetPointer());
		{
			IList* pPairedDeviceList = __btManager.GetPairedDeviceListN();
			// Get the paired device element at the index 0
			for ( int i=0; i< pPairedDeviceList->GetCount(); i++) {
				BluetoothDevice* pPairedDevice = (BluetoothDevice*)pPairedDeviceList->GetAt(i);
				// Get information from the paired device
				String pairedDeviceName = pPairedDevice->GetName();
				AppLog("Bluetooth: paired(%d): %S.\n", i, pairedDeviceName.GetPointer());
			}
			pPairedDeviceList->RemoveAll(true); // Clear resources after use to prevent memory leaks

			delete pPairedDeviceList;
		}
	}
#endif

	SetFormBackEventListener(this);

	return true;
}

result
BluetoothDebugForm::OnInitializing(void)
{

	return E_SUCCESS;
}

void
BluetoothDebugForm::OnActionPerformed(const Control& source, int actionId)
{
	switch (actionId)
	{
	case ID_OPTION_DONE:
	{
		SceneManager* pSceneManager = SceneManager::GetInstance();
		AppAssert(pSceneManager);
		pSceneManager->GoBackward(BackwardSceneTransition());
	}
		break;

	case ID_BUTTON_CONNECT: Connect_KBT1();	break;
	case ID_BUTTON_DISCONNECT: Disconnect_KBT1();	break;
	case ID_BUTTON_ACCEPTOR: Initialize_SppAcceptor();break;
	case ID_BUTTON_PING: KBT1_Ping(); break;
	case ID_BUTTON_MOVE_HEAD: KBT1_Move_Head();	break;
	case ID_BUTTON_PLAYMOTION_1: KBT1_PlayMotion(1);	break;
	case ID_BUTTON_PLAYMOTION_2: KBT1_PlayMotion(2);	break;
	case ID_BUTTON_PLAYMOTION_3: KBT1_PlayMotion(3);	break;
	case ID_BUTTON_PLAYMOTION_4: KBT1_PlayMotion(4);	break;
	case ID_BUTTON_PLAYMOTION_5: KBT1_PlayMotion(5);	break;
	case ID_BUTTON_PLAYMOTION_6: KBT1_PlayMotion(6);	break;
	case ID_BUTTON_PLAYMOTION_7: KBT1_PlayMotion(7);	break;
	case ID_BUTTON_PLAYMOTION_8: KBT1_PlayMotion(8);	break;

	default:
		break;
	}
}

void
BluetoothDebugForm::ShowTimeoutMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text, unsigned long timeout)
{
	MessageBox messageBox;
	int result = 0;
	messageBox.Construct(title, text, MSGBOX_STYLE_NONE, timeout);
	messageBox.ShowAndWait(result);
}

void
BluetoothDebugForm::OnFormBackRequested(Form& source)
{
    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    pSceneManager->GoBackward(BackwardSceneTransition());
}

bool
BluetoothDebugForm::AddMainControl(ControlType type, const String& text, int firstActionId /*= -1*/, int secondActionId /*= -1*/)
{
    static const int ACTION_BUTTON_HEIGHT = 60;	//72;
    static const int EDIT_FIELD_HEIGHT = 96;
    static const int CHECK_BUTTON_HEIGHT = 96;
    static const int DEVICE_NAME_LIMIT = 64;
    static const int LEFT_MARGIN = 20;
    static const int RIGHT_MARGIN = 40;
    static const int DISPLAY_BUTTON_COUNT = ButtonActionType::COUNT - 1;
    static Rectangle clientRect = GetClientAreaBounds();
    //static int emptySpaceHeight = (clientRect.height - ((DISPLAY_BUTTON_COUNT * ACTION_BUTTON_HEIGHT) + EDIT_FIELD_HEIGHT + CHECK_BUTTON_HEIGHT)) / (DISPLAY_BUTTON_COUNT + 1); // button + edit filed
    static int emptySpaceHeight = 10;
    static int currentHeight = 0;

    /*
    if (type == CONTROL_EDIT_FILED)
    {
        __pEditFieldDeviceName = new EditField();
        __pEditFieldDeviceName->Construct(Rectangle(LEFT_MARGIN, currentHeight, clientRect.width - RIGHT_MARGIN, EDIT_FIELD_HEIGHT),
                                          EDIT_FIELD_STYLE_NORMAL, INPUT_STYLE_FULLSCREEN, true, DEVICE_NAME_LIMIT, GROUP_STYLE_NONE);
        __pEditFieldDeviceName->SetTitleText(text);
        __pEditFieldDeviceName->AddTextEventListener(*this);
        AddControl(*__pEditFieldDeviceName);

        currentHeight += EDIT_FIELD_HEIGHT + emptySpaceHeight;
    }
    else */ if (type == CONTROL_BUTTON)
    {
        Button* pButton = new Button();
        pButton->Construct(Rectangle(LEFT_MARGIN, currentHeight, clientRect.width - RIGHT_MARGIN, ACTION_BUTTON_HEIGHT), text);
        pButton->SetActionId(firstActionId);
        pButton->AddActionEventListener(*this);
        AddControl(*pButton);
        __controlList.Add(pButton);

        currentHeight += ACTION_BUTTON_HEIGHT + emptySpaceHeight;
    }
    /* else if (type == CONTROL_CHECK_BUTTON_STYLE_RADIO)
    {
        CheckButton* pCheckButton = new CheckButton();
        pCheckButton->Construct(Rectangle(LEFT_MARGIN, currentHeight, clientRect.width - RIGHT_MARGIN, CHECK_BUTTON_HEIGHT), CHECK_BUTTON_STYLE_ONOFF_SLIDING, BACKGROUND_STYLE_DEFAULT, false, text);
        pCheckButton->SetActionId(firstActionId, secondActionId);
        pCheckButton->AddActionEventListener(*this);
        AddControl(*pCheckButton);
        __controlList.Add(pCheckButton);

        currentHeight += CHECK_BUTTON_HEIGHT + emptySpaceHeight;
    }
	*/

    return true;
}


result
BluetoothDebugForm::Connect_KBT1()
{

	if (!__isSppConstructed) {
		if (!IsFailed(__sppInitiator.Construct(*this))) __isSppConstructed = true;
	}
	{
		// The application must implement the IBluetoothSppInitiatorEventListener interface
		//if (!IsFailed(__sppInitiator.Construct(*this)))
		if ( !__isBtKBTConnectionResponded )
		{
			IList* pPairedDeviceList = __btManager.GetPairedDeviceListN();
			// Get the paired device element at the index 0
			for ( int i=0; i< pPairedDeviceList->GetCount(); i++) {
				BluetoothDevice* pPairedDevice = (BluetoothDevice*)pPairedDeviceList->GetAt(i);
				// Get information from the paired device
				String pairedDeviceName = pPairedDevice->GetName();
				if (  pairedDeviceName == "KBT-1" || pairedDeviceName == "KBT-1-2" /* || pairedDeviceName == "Tizen" || pairedDeviceName == "Tizen2" || pairedDeviceName == "Tizen3" */ ) {
					AppLog("Bluetooth: found paired %S in list. trying to connect...\n", pairedDeviceName.GetPointer());
					//Connect to the SPP acceptor.
					if (!IsFailed(__sppInitiator.Connect(*pPairedDevice))) {
						__isBtKBTReady = true;
						AppLog("Bluetooth: %S connected!\n", pairedDeviceName.GetPointer());
						break;
					}
				}
			}
			pPairedDeviceList->RemoveAll(true); // Clear resources after use to prevent memory leaks
			delete pPairedDeviceList;
			if (!__isBtKBTReady) {
				AppLog("Bluetooth: KBT-1 not found or not connected!");
				return E_FAILURE;
			}
		}
	}

	return E_SUCCESS;
}


result
BluetoothDebugForm::Disconnect_KBT1()
{
	//Disconnect from the SPP acceptor:
	result r = __sppInitiator.Disconnect();
	__isBtKBTReady = false;

	return r;
}

result
BluetoothDebugForm::Initialize_SppAcceptor()
{

	{
		// The application must implement the IBluetoothSppInitiatorEventListener interface
		if (!IsFailed(__sppAcceptor.Construct(*this)))
		{

			if (IsFailed(__sppAcceptor.StartService())) {
				AppLog("Bluetooth: Cannot start SPP Acceptor service!");
				return E_FAILURE;
			}
		}

	}

	return E_SUCCESS;
}

void
BluetoothDebugForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{

}

void BluetoothDebugForm::OnSppConnectionResponded(result r)
{
	AppLog("OnSppConnectionResponded: r=%d", r);	// E_SYSTEM = -1610609822 =
	if (!IsFailed(r)) {
		// connected without error
		__isBtKBTConnectionResponded = true;
		//static_cast<Button*>(__controlList.GetAt(ButtonActionType2::CONNECT))->SetEnabled(false);
		//static_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetEnabled(true);
		static_cast<Button*>(__controlList.GetAt(ButtonActionType2::CONNECT))->SetColor(BUTTON_STATUS_NORMAL, Color::GetColor(COLOR_ID_GREEN));
		//static_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetColor(BUTTON_STATUS_NORMAL,Color(0,0,0));
	} else {
		//static_cast<Button*>(__controlList.GetAt(ButtonActionType2::CONNECT))->SetEnabled(true);
		//static_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetEnabled(false);
		static_cast<Button*>(__controlList.GetAt(ButtonActionType2::CONNECT))->SetColor(BUTTON_STATUS_NORMAL, Color::GetColor(COLOR_ID_RED));
		//static_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetColor(BUTTON_STATUS_NORMAL,Color(255,0,0));
	}
	RequestRedraw();
}

void BluetoothDebugForm::OnSppConnectionRequested(const Tizen::Net::Bluetooth::BluetoothDevice &device)
{
	AppLog("OnSppConnectionRequested:");	// E_SYSTEM = -1610609822 =
	int r = __sppAcceptor.AcceptConnection();
}

void BluetoothDebugForm::OnSppDataReceived(Tizen::Base::ByteBuffer &buffer)
{
	AppLog("OnSppDataReceived: len=%d", buffer.GetLimit() /*, buffer.GetPointer() */);
	if (buffer.GetLimit() == 4) {	// possible response from RCB4 vir KBT-1
		byte res[4];
		buffer.GetArray(res, 0, 4);
		if (res[2] == 0x06) {
			AppLog("ACK Received from KBT-1");
			static_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetColor(BUTTON_STATUS_NORMAL, Color::GetColor(COLOR_ID_BLUE));
		} else if ( res[2] == 0x15) {
			AppLog("NAK Received from KBT-1");
			static_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetColor(BUTTON_STATUS_NORMAL, Color::GetColor(COLOR_ID_RED));
		}
	} else {
		/* unexpected response */
		static_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetColor(BUTTON_STATUS_NORMAL, Color::GetColor(COLOR_ID_YELLOW));
	}
	RequestRedraw();
}

void BluetoothDebugForm::OnSppDisconnected(result r)
{
	AppLog("OnSppDisconnected: r=%d", r);
	__isBtKBTConnectionResponded = false;
	//static_cast<Button*>(__controlList.GetAt(ButtonActionType2::CONNECT))->SetEnabled(true);
	//sstatic_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetEnabled(false);
	static_cast<Button*>(__controlList.GetAt(ButtonActionType2::CONNECT))->SetColor(BUTTON_STATUS_NORMAL, Color::GetColor(COLOR_ID_RED));
	//static_cast<Button*>(__controlList.GetAt(ButtonActionType2::DISCONNECT))->SetColor(BUTTON_STATUS_NORMAL,Color(255,0,0));
	RequestRedraw();
}


byte
BluetoothDebugForm::RCB4_calc_checksum(byte *cmd, int len)
{
	byte sum = 0;
	for ( int i = 0; i < len; i++) sum += cmd[i];
	return sum;
}


/* send request ack command to KBT-1/RCB4 */
result
BluetoothDebugForm::KBT1_Ping()
{
	result r = E_SUCCESS;
	ByteBuffer sendBuffer;
	//byte b[] = { 0x0d, 0x00, 0x02, 0x50, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x63 };	// HTH KRC Num8
	//byte b2[]= { 0x0d, 0x00, 0x02, 0x50, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62 };	// HTH KRC Num8
	//byte b[]= { 'a', 'b', 'c', 'd' };	// test ascii data
	//byte b[]= {  0x80, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01 };	// Num8 by KRC commander?
	//byte b;
	//b = 0x04; sendBuffer.SetByte(b);
	//b = 0xfe; sendBuffer.SetByte(b);
	//b = 0x06; sendBuffer.SetByte(b);
	//b = 0x08; sendBuffer.SetByte(b);
	byte b[4] = { 0x04, 0xfe, 0x06, 0x08 };		// ping command; request ack
	sendBuffer.Construct(sizeof(b));
	sendBuffer.SetArray(b, 0, sizeof(b));
	sendBuffer.Flip();
	r = __sppInitiator.SendData(sendBuffer);

	AppLogIf(!IsFailed(r), "KBT1_Ping: send OK");
	AppLogIf( IsFailed(r), "KBT1_Ping: send failed");

	// Receive by OnSppDataReceived
	return r;
}


/* send head move command to KBT1/RCB4 */
result
BluetoothDebugForm::KBT1_Move_Head()
{
	result r = E_SUCCESS;
	byte RCB4_cmd_Move_Head[]= { 0x07, 0x0f, 0x00, 0x01, 0xac, 0x0d, 0xd0 };	// Move single servo ICS 0 to poition 3500
	ByteBuffer sendBuffer;
	sendBuffer.Construct(sizeof(RCB4_cmd_Move_Head));
	sendBuffer.SetArray(RCB4_cmd_Move_Head, 0, sizeof(RCB4_cmd_Move_Head));
	sendBuffer.Flip();
	r = __sppInitiator.SendData(sendBuffer);

	AppLogIf(!IsFailed(r), "KBT1_Move_Head: send OK");
	AppLogIf( IsFailed(r), "KBT1_Move_Head: send failed");

	// Response should be receive by OnSppDataReceived
	return r;
}


/* send play motion command to KBT1/RCB4 */
result
BluetoothDebugForm::KBT1_PlayMotion(int index)
{
	result r = E_SUCCESS;
	byte RCB4_cmd_suspend_motion[] = { 0x09, 0x00, 0x02, 0x00, 0x00, 0x00, 0x10, 0x83, 0x9e };		// suspend motion
	byte RCB4_cmd_reset_state[] = { 0x11, 0x00, 0x02, 0x02, 0x00, 0x00, 0x4b, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64 };	// reset program counter and EEPROM refresh flag
	byte RCB4_cmd_set_move_address[] = { 0x07, 0x0c, 0xb8, 0x0b, 0x00, 0x00, 0xd6 };		// set move address by call instruction
	byte RCB4_cmd_start_motion[] = { 0x09, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1b, 0x83, 0xa9 };		// restart motion

	ByteBuffer sendBuffer;
	sendBuffer.Construct(256);

	// suspend motion
	sendBuffer.SetArray(RCB4_cmd_suspend_motion, 0, sizeof(RCB4_cmd_suspend_motion));
	sendBuffer.Flip();
	r = __sppInitiator.SendData(sendBuffer);

	// reset state
	sendBuffer.Clear();
	sendBuffer.SetArray(RCB4_cmd_reset_state, 0, sizeof(RCB4_cmd_reset_state));
	sendBuffer.Flip();
	r |= __sppInitiator.SendData(sendBuffer);

	// set move address
	//  calculate start address
	unsigned long motion_addr;
	motion_addr = 3000 + 2048*(index-1);	// For HeartToHeart4 V2.0, V2.1 only
	RCB4_cmd_set_move_address[2] = motion_addr & 0xff;
	RCB4_cmd_set_move_address[3] = (motion_addr >>  8) & 0xff;
	RCB4_cmd_set_move_address[4] = (motion_addr >> 16) & 0xff;
	RCB4_cmd_set_move_address[sizeof(RCB4_cmd_set_move_address)-1] = RCB4_calc_checksum(RCB4_cmd_set_move_address, sizeof(RCB4_cmd_set_move_address)-1);
	sendBuffer.Clear();
	sendBuffer.SetArray(RCB4_cmd_set_move_address, 0, sizeof(RCB4_cmd_set_move_address));
	sendBuffer.Flip();
	r |= __sppInitiator.SendData(sendBuffer);

	// start motion
	sendBuffer.Clear();
	sendBuffer.SetArray(RCB4_cmd_start_motion, 0, sizeof(RCB4_cmd_start_motion));
	sendBuffer.Flip();
	r |= __sppInitiator.SendData(sendBuffer);

	AppLogIf(!IsFailed(r), "KBT1_PlayMotion(%d): send OK", index);
	AppLogIf( IsFailed(r), "KBT1_PlayMotion(%d): send failed", index);

	// Receive by OnSppDataReceived
	return r;
}



/* Kondo KHR-4HV memo
 *  Dual USB adapter HS cable configuration
 *   Pin 1: Rx(White), 2: Tx(Red), 3: G(Black)
 *
 *  KBT-1 SW configuration; RCB-4 slave mode
 *   SW1: ON-OFF, SW2: OFF-ON
 */

/* Tizen 2.2 Bluetooth stack patch
 *  Issue: Tizen OS Bluetooth SPP Initiator cannot pass binary data.
 *  following patch fix above limitation.
 *
 * /rpmbuild/BUILD/bluetooth-frwk-0.2.57/bt-service/
 *  rpmbuild -bb --target=arm ./bluetooth-frwk.spec
 *
 * Patch source and build
 *  bt-api/bt-rfcomm-client.c: bluetooth_rfcomm_write(): change g_strlcpy() to memcpy()
 *  bt-service/bt-service-rfcomm-client.c: _bt_rfcomm_write(): remove lines of g_utf8_validate() and new_length
 *
 * Recompile or replace binary. File in this project
 *  /usr/lib/libbluetooth-api.so.1.0.0: c819fd967b6a6f5b3639820eee9d066f
 *  /usr/bin/bt-service: b7d5ec3cee3fd135d6270d013ff0d11a
 */

/*
 * patch
--- bluetooth-frwk-0.2.57/bt-api/bt-rfcomm-client.c.org 2013-10-26 15:07:07.225263814 +0900
+++ bluetooth-frwk-0.2.57/bt-api/bt-rfcomm-client.c     2013-10-26 20:35:52.495265904 +0900
@@ -139,7 +139,7 @@

        buffer = g_malloc0(length + 1);

-       g_strlcpy(buffer, buf, length + 1);
+       memcpy(buffer, buf, length + 1);

        g_array_append_vals(in_param1, &fd, sizeof(int));
        g_array_append_vals(in_param2, &length, sizeof(int));


 */
