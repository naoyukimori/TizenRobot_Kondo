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
	: __isBtKBTReady(false)
	, __isBtKBTConnectionResponded(false)
{

}

BluetoothDebugForm::~BluetoothDebugForm(void)
{
	if ( __isBtKBTReady ) {
		//Disconnect from the SPP acceptor:
		__sppInitiator.Disconnect();
		__isBtKBTReady = false;
	}
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

	AddMainControl(CONTROL_BUTTON, L"Ping", ID_BUTTON_PING);
	AddMainControl(CONTROL_BUTTON, L"Go Forward", ID_BUTTON_GOFORWARD);
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
		ShowTimeoutMessageBox(L"Bluetooth Scan", L"Please Activate Bluetooth", 3000);
		return false;
	}

	__btManager.SetBluetoothDeviceListener(this);

#if 1
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

	{
		// The application must implement the IBluetoothSppInitiatorEventListener interface
		if (!IsFailed(__sppInitiator.Construct(*this)))
		{
			IList* pPairedDeviceList = __btManager.GetPairedDeviceListN();
			// Get the paired device element at the index 0
			for ( int i=0; i< pPairedDeviceList->GetCount(); i++) {
				BluetoothDevice* pPairedDevice = (BluetoothDevice*)pPairedDeviceList->GetAt(i);
				// Get information from the paired device
				String pairedDeviceName = pPairedDevice->GetName();
				if ( pairedDeviceName == "KBT-1" || pairedDeviceName == "KBT-1-2") {
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
				return true;
			}
		}

		//Disconnect from the SPP acceptor:
		//sppInitiator.Disconnect();
	}

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

	case ID_BUTTON_PING:
	{
		KBT1_Ping();
	}
		break;

	case ID_BUTTON_GOFORWARD:
	{
		KBT1_GoForward();
	}
		break;

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
    static const int ACTION_BUTTON_HEIGHT = 72;
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


void
BluetoothDebugForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{

}

void
BluetoothDebugForm::OnBluetoothDiscoveryDone(bool isCompleted)
{

}

void
BluetoothDebugForm::OnBluetoothDiscoveryStarted(result r)
{

}

void
BluetoothDebugForm::OnBluetoothRemoteDeviceFoundN(Tizen::Net::Bluetooth::BluetoothDevice* pFoundDevice)
{

}

void BluetoothDebugForm::OnSppConnectionResponded(result r)
{
	AppLog("OnSppConnectionResponded: r=%d", r);	// E_SYSTEM = -1610609822 =
	if (!IsFailed(r)) __isBtKBTConnectionResponded = true;
}

void BluetoothDebugForm::OnSppDataReceived(Tizen::Base::ByteBuffer &buffer)
{
	AppLog("OnSppDataReceived: len=%d, %S", buffer.GetLimit(), buffer.GetPointer());
}

void BluetoothDebugForm::OnSppDisconnected(result r)
{
	AppLog("OnSppDisconnected: r=%d", r);
	__isBtKBTConnectionResponded = false;
}

result
BluetoothDebugForm::KBT1_Ping()
{
	result r = E_SUCCESS;
	//Send data when the connection is established:
	ByteBuffer sendBuffer;
	sendBuffer.Construct(100);
	//sendTextMsg = ['0x04', '0xFE', '0x06', '0x08' ];	// ping command
	byte b;
	b = 0x04; sendBuffer.SetByte(b);
	b = 0xfe; sendBuffer.SetByte(b);
	b = 0x06; sendBuffer.SetByte(b);
	b = 0x08; sendBuffer.SetByte(b);
	sendBuffer.Flip();
	r = __sppInitiator.SendData(sendBuffer);

	AppLogIf(!IsFailed(r), "KBT1_Ping: send OK");
	AppLogIf( IsFailed(r), "KBT1_Ping: send failed");

	// Receive by OnSppDataReceived
	return r;
}


result
BluetoothDebugForm::KBT1_GoForward()
{
	result r = E_SUCCESS;
	//Send data when the connection is established:
	ByteBuffer sendBuffer;
	sendBuffer.Construct(100);
	//sendTextMsg = ['80 00 01 40 40 40 40 01' ];	// NumPad8; go forward?? now work
	//sendTextMsg = ['07 0f 00 01 ac 0d d0' ];	// Move single servo ICS 0 to poition 3500
	byte b;
	b = 0x07; sendBuffer.SetByte(b);
	b = 0x0f; sendBuffer.SetByte(b);
	b = 0x00; sendBuffer.SetByte(b);
	b = 0x01; sendBuffer.SetByte(b);
	b = 0xac; sendBuffer.SetByte(b);
	b = 0x0d; sendBuffer.SetByte(b);
	b = 0xd0; sendBuffer.SetByte(b);
	r = __sppInitiator.SendData(sendBuffer);

	AppLogIf(!IsFailed(r), "KBT1_MoveICS1: send OK");
	AppLogIf( IsFailed(r), "KBT1_MoveICS1: send failed");

	// Receive by OnSppDataReceived
	return r;
}
