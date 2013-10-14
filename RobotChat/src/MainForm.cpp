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

#include "MainForm.h"
#include "DeviceListForm.h"
#include "SceneRegister.h"
#include "FormFactory.h"

using namespace std;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Net::Wifi;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Net::Bluetooth;

MainForm::MainForm(void)
	: __pEditFieldDeviceName(null)
	, __pPopupWait(null)
	, __pPopupScan(null)
	, __pPopupConnect(null)
	, __pLabel(null)
	, __pWifiDirectLocalDevice(null)
	, __currentState(STATE_ACTIVATED)
	, __isWiifDirectSupported(true)
	, __isWifiDeactivating(false)
{
}

MainForm::~MainForm(void)
{
	delete __pPopupWait;
	delete __pPopupScan;
	delete __pPopupConnect;
}

bool
MainForm::Initialize(void)
{
	Construct(FORM_STYLE_PORTRAIT_INDICATOR | FORM_STYLE_HEADER | FORM_STYLE_FOOTER);

	SetName(FORM_MAIN);

	__pWifiDirectLocalDevice.reset(WifiDirectDeviceManager::GetWifiDirectDeviceN());

	if (__pWifiDirectLocalDevice != null)
	{
		__pWifiDirectLocalDevice->AddWifiDirectDeviceListener(*this);
	}

	result r = GetLastResult();

	if (r == E_UNSUPPORTED_OPERATION)
	{
		__isWiifDirectSupported = false;
	}

	__wifiManager.Construct(*this);

	return true;
}


result
MainForm::OnInitializing(void)
{
	Header* pHeader = GetHeader();
	if (pHeader != null)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
		pHeader->SetTitleText(L"Robot chat");
	}

	Footer* pFooter = GetFooter();
	if (pFooter != null)
	{
		pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);
		pFooter->AddActionEventListener(*this);
	}

	// Setup back event listener
	SetFormBackEventListener(this);

	// Check whether the Wi-Fi Direct is supported.
	if (!__isWiifDirectSupported)
	{
		const int UNSUPPORTED_LABEL_HEIGHT = 100;
		Label* pLabelUnsupportedOperation = new Label();
		pLabelUnsupportedOperation->Construct(Rectangle(0, (GetClientAreaBounds().height - UNSUPPORTED_LABEL_HEIGHT) / 2, GetClientAreaBounds().width,
				UNSUPPORTED_LABEL_HEIGHT), "WiFi Direct Is Not Supported On This Device");

		pLabelUnsupportedOperation->SetTextHorizontalAlignment(ALIGNMENT_CENTER);
		pLabelUnsupportedOperation->SetTextVerticalAlignment(ALIGNMENT_MIDDLE);
		AddControl(*pLabelUnsupportedOperation);
		return E_SUCCESS;
	}
	TryReturn(UpdateCurrentState()== true, E_FAILURE, "Failed to initialize the current state");
	AddMainControl(CONTROL_BUTTON, L"Bluetooth", ID_BUTTON_BLUETOOTH);
	AddMainControl(CONTROL_CHECK_BUTTON_STYLE_RADIO, L"Wi-Fi Direct", ID_BUTTON_ACTIVATE, ID_BUTTON_DEACTIVATE);
	AddMainControl(CONTROL_EDIT_FILED, L"Device Name");
	AddMainControl(CONTROL_BUTTON, L"Connect", ID_BUTTON_CONNECT);
	AddMainControl(CONTROL_BUTTON, L"Create Autonomous Group", ID_BUTTON_CREATE_AUTO_GROUP);
	AddMainControl(CONTROL_BUTTON, L"Leave Group", ID_BUTTON_LEAVE_GROUP);
	AddMainControl(CONTROL_BUTTON, L"Start Chat", ID_BUTTON_START_CHAT);
	AddPopup();

	const int MAX_PORT_LENGTH = 5;
	__chatPortNumber.Format(MAX_PORT_LENGTH, L"%d", DEFAULT_CHAT_PORT);

	UpdateLocalDeviceName();
	UpdateControl();
	return E_SUCCESS;
}

void
MainForm::ShowMessageBox(const String& title, const String& text)
{
	static const int MESSAGE_BOX_TIMEOUT = 1500;
	MessageBox messageBox;
	int modalResult = 0;
	messageBox.Construct(title, text, MSGBOX_STYLE_NONE, MESSAGE_BOX_TIMEOUT);
	messageBox.ShowAndWait(modalResult);
}

void
MainForm::ShowDeviceInfoForm(void)
{
    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    WifiDirectDeviceInfo* pLocalDeviceInfo = __pWifiDirectLocalDevice->GetLocalDeviceInfoN();

    if (!pLocalDeviceInfo)
    {
        ShowMessageBox(L"Device Info", L"Failed To Get Device Info");
        return;
    }
    else
    {
        ArrayList* pArgs = new ArrayList();
        pArgs->Construct();
        pArgs->Add(*pLocalDeviceInfo);
        pSceneManager->GoForward(ForwardSceneTransition(SCENE_DEVICE_INFO_FORM), pArgs);
    }
}

void
MainForm::ShowGroupConfigurationForm(void)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	ArrayList* pArgs = new ArrayList(SingleObjectDeleter);

	WifiDirectGroupInfo* pWifiDirectGroupInfo = __pWifiDirectLocalDevice->GetGroupSettingInfoN();

	pArgs->Add(*pWifiDirectGroupInfo);

	pSceneManager->GoForward(ForwardSceneTransition(SCENE_GROUP_CONFIG_FORM), pArgs);
}
void
MainForm::ShowChatForm(void)
{
    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    ArrayList* pArgs = new ArrayList();

    pArgs->Add(*__pWifiDirectLocalDevice);
    pArgs->Add(__chatPortNumber);

    pSceneManager->GoForward(ForwardSceneTransition(SCENE_CHAT_FORM), pArgs);
}
void
MainForm::ShowChatPortConfigurationForm()
{
    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    ArrayList* pArgs = new ArrayList();

    pArgs->Add(__chatPortNumber);

    pSceneManager->GoForward(ForwardSceneTransition(SCENE_CHAT_PORT_CONFIG_FORM), pArgs);
}
void
MainForm::ShowWpsConfigurationModeForm()
{
    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    unique_ptr<String> pString(new String());

    pString->Format(2, L"%1d", __pWifiDirectLocalDevice->GetWpsConfigurationModePreference());

    ArrayList* pArgs = new ArrayList(SingleObjectDeleter);

    pArgs->Add(*pString.release());

    pSceneManager->GoForward(ForwardSceneTransition(SCENE_WPS_CONFIG_MODE_FORM), pArgs);
}
void
MainForm::ShowPeerDeviceListForm()
{
    IList* pArgs = null;
    String* pString = null;
    unique_ptr<WifiDirectDeviceInfo> pLocalDeviceInfo(__pWifiDirectLocalDevice->GetLocalDeviceInfoN());

    TryReturnVoid(pLocalDeviceInfo != null, "Failed to get local device information");

    if (pLocalDeviceInfo->GetGroupMemberType() == WIFI_DIRECT_GROUP_MEMBER_TYPE_OWNER)
    {
        pArgs = __pWifiDirectLocalDevice->GetGroupClientInfoListN();

        if (pArgs == null)
        {
            ShowMessageBox(L"Peer List", L"Failed to get peer devices information");
            return;
        }
        pString = new String(L"true");
    }
    else
    {
        WifiDirectDeviceInfo* pDeviceInfo = __pWifiDirectLocalDevice->GetGroupOwnerInfoN();
        TryReturnVoid(pDeviceInfo != null, "Failed to get group owner device information");

        pArgs = new ArrayList();
        pArgs->Add(*pDeviceInfo);
        pString = new String(L"false");
    }
    pArgs->InsertAt(*pString, 0);

    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    pSceneManager->GoForward(ForwardSceneTransition(SCENE_PEER_DEVICE_LIST_FORM), pArgs);
}

void
MainForm::ShowWaitPopup(const Tizen::Base::String& text)
{
	__pLabel->SetText(text);
	__pPopupWait->SetShowState(true);
	__pPopupWait->Show();
}

void
MainForm::HideWaitPopup(void)
{
	__pPopupWait->SetShowState(false);
	Draw(true);
}

void
MainForm::ShowScanPopup(void)
{
    __pPopupScan->SetShowState(true);
    __pPopupScan->Show();
}

void
MainForm::HideScanPopup(void)
{
    __pPopupScan->SetShowState(false);
    Draw(true);
}

void
MainForm::ShowConnectPopup(void)
{
    __pPopupConnect->SetShowState(true);
    __pPopupConnect->Show();

}

void
MainForm::HideConnectPopup(void)
{
    __pPopupConnect->SetShowState(false);
    Draw(true);
}



void
MainForm::ShowMainForm(void)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoForward(ForwardSceneTransition(SCENE_MAIN_FORM, SCENE_TRANSITION_ANIMATION_TYPE_NONE,
            SCENE_HISTORY_OPTION_NO_HISTORY, SCENE_DESTROY_OPTION_DESTROY));
}

void
MainForm::OnActionPerformed(const Control& source, int actionId)
{
	result r = E_SUCCESS;

	switch (actionId)
	{
	case ID_BUTTON_ACTIVATE:
		if (__wifiManager.IsActivated())
		{
			r = __wifiManager.Deactivate();

			if (r == E_SUCCESS)
			{
				__isWifiDeactivating = true;
				ShowWaitPopup(L"Activating...");
			}
			else
			{
				ShowMessageBox(L"Activate", L"Failed to Activate");
			}
		}
		else
		{
			Activate();
		}
		break;
	case ID_BUTTON_DEACTIVATE:
		r = __pWifiDirectLocalDevice->Deactivate();
		if (r == E_SUCCESS)
		{
			ShowWaitPopup(L"Deactivating...");
		}
		break;
	case ID_BUTTON_CONNECT:
    {
        SceneManager* pSceneManager = SceneManager::GetInstance();
        AppAssert(pSceneManager);
        pSceneManager->GoForward(ForwardSceneTransition(SCENE_SCAN_RESULT_DEVICE_LIST_FORM));
        Scan();
    }
		break;
	case ID_BUTTON_CANCEL_SCAN:
		r = __pWifiDirectLocalDevice->CancelScan();
		if (r != E_SUCCESS)
		{
			ShowMessageBox(L"Cancel Scan", L"Failed To Cancel Scan");
		}
		else if (r == E_IN_PROGRESS)
		{
			ShowMessageBox(L"Cancel Scan", L"Cancel Scanning Is In Progress");
		}
		break;
	case ID_BUTTON_CANCEL_CONNECT:
		r = __pWifiDirectLocalDevice->CancelConnect();
		if (r == E_SYSTEM || r == E_INVALID_OPERATION)
		{
			ShowMessageBox(L"Cancel Connect", L"Failed To Cancel Connect");
		}
		else if (r == E_IN_PROGRESS)
		{
			ShowMessageBox(L"Cancel Connect", L"Cancel Connecting Is In Progress");
		}
		break;
    case ID_BUTTON_CREATE_AUTO_GROUP:
        r = __pWifiDirectLocalDevice->CreateAutonomousGroup();
        if (r == E_SUCCESS)
        {
            ShowWaitPopup(L"Creating Autonomous Group...");
        }
        break;
    case ID_BUTTON_LEAVE_GROUP:
        r = __pWifiDirectLocalDevice->LeaveGroup();
        if (r == E_SUCCESS)
        {
            ShowWaitPopup(L"Leaving Group...");
        }
        break;
	case ID_BUTTON_START_CHAT:
	    ShowChatForm();
		break;

	case ID_FOOTER_DEVICE_INFO:
	    ShowDeviceInfoForm();
	    __pWifiDirectLocalDevice->GetGroupSettingInfoN();
		break;
	case ID_FOOTER_GROUP_CONFIG:
		ShowGroupConfigurationForm();
		break;
	case ID_FOOTER_CHAT_PORT_CONFIG:
	    ShowChatPortConfigurationForm();
	    break;
	case ID_FOOTER_WPS_CONFIG:
	    ShowWpsConfigurationModeForm();
	    break;
	case ID_FOOTER_PEER_DEVICE_LIST:
	    ShowPeerDeviceListForm();
		break;
	default:
		break;
	}
	Draw(true);
}

void
MainForm::OnTextValueChanged(const Control& source)
{
	if (source.Equals(*__pEditFieldDeviceName))
	{
		if (__pEditFieldDeviceName->GetTextLength() == 0)
		{
			ShowMessageBox(L"Error", L"Device Name Cannot Be Empty");
			UpdateLocalDeviceName();
		}
		else
		{
			UpdateLocalDeviceName(__pEditFieldDeviceName->GetText());
		}
	}
}

void
MainForm::OnFormBackRequested(Form& source)
{
	UiApp* pApp = UiApp::GetInstance();
	AppAssert(pApp);
	pApp->Terminate();
}

void
MainForm::OnWifiDirectDeviceActivated(WifiDirectDeviceId localDeviceId, result r)
{
	String message;

	if (r == E_SUCCESS)
	{
		message.Append(L"WiFi Direct Activated");
	}
	else
	{
		message.Append(L"Failed To Activate");
	}


    UpdateCurrentState();
    UpdateControl();

    HideWaitPopup();
	ShowMessageBox("Activate", message);
}

void
MainForm::OnWifiDirectDeviceDeactivated(WifiDirectDeviceId localDeviceId, result r)
{
	String message;

	if (r == E_SUCCESS)
	{
		message.Append(L"WiFi Direct Deactivated");

		Form* pForm = Application::GetInstance()->GetAppFrame()->GetFrame()->GetCurrentForm();
		if (pForm != this)
	    {
		    ShowMainForm();
	    }

	}
	else
	{
		message.Append(L"Failed To Deactivate");
	}

	UpdateCurrentState();
	UpdateControl();
	UpdateLocalDeviceName();

	HideWaitPopup();
	ShowMessageBox(L"Deactivate", message);
}

void
MainForm::OnWifiDirectConnected(WifiDirectDeviceId localDeviceId, const WifiDirectDeviceInfo& remoteDeviceInfo, result r)
{
    String message;

    if (r == E_SUCCESS)
    {
        const int MAX_LENGTH = 50;
        message.Format(MAX_LENGTH, L"WiFi Direct Connected with %ls", remoteDeviceInfo.GetDeviceName().GetPointer());
    }
    else
    {
        message.Append(L"Failed To Connect");
    }

    UpdateCurrentState();
    UpdateControl();

    HideConnectPopup();
    ShowMessageBox(L"Connect", message);

}
void
MainForm::OnWifiDirectDisconnected(WifiDirectDeviceId localDeviceId, const String& peerMacAddress, result r)
{
    String message;

    if (r == E_SUCCESS)
    {
        const int MAX_LENGTH = 50;
        message.Format(MAX_LENGTH, L"WiFi Direct Disconnected with %ls", peerMacAddress.GetPointer());
    }
    else
    {
        message.Append(L"Failed To Disconnect");
    }

    UpdateCurrentState();
    UpdateControl();

    HideWaitPopup();
    ShowMessageBox(L"Disconnect", message);
}

void
MainForm::OnWifiDirectAutonomousGroupCreated(WifiDirectDeviceId localDeviceId, result r)
{
    String message;

    if (r == E_SUCCESS)
    {
        message.Append(L"WiFi Direct Autonomous Group Created");
    }
    else
    {
        message.Append(L"Failed To Create Autonomous Group");
    }

    __currentState = STATE_AUTO_GROUP_OWNER;
    UpdateControl();

    HideWaitPopup();
    ShowMessageBox("Create Autonomous Group", message);
}

void
MainForm::OnWifiDirectGroupLeft(WifiDirectDeviceId localDeviceId, result r)
{
	String message;

	if (r == E_SUCCESS)
	{
		message.Append(L"WiFi Direct Group Left");

		Form* pForm = Application::GetInstance()->GetAppFrame()->GetFrame()->GetCurrentForm();
        if (pForm != this)
        {
            ShowMainForm();
        }
	}
	else
	{
		message.Append(L"Failed To Left Group");
	}

	UpdateCurrentState();
	UpdateControl();

	HideWaitPopup();
    ShowMessageBox("Group Left", message);
}

void
MainForm::OnWifiDirectScanCompletedN(WifiDirectDeviceId localDeviceId, IList* pWifiDirectDeviceInfoList, result r)
{
    DeviceListForm* pDeviceListForm = static_cast< DeviceListForm* >(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl(FORM_DEVICE_LIST));
    HideScanPopup();

	if (r == E_SUCCESS && pDeviceListForm != null)
	{
		pDeviceListForm->UpdateScanResult(pWifiDirectDeviceInfoList);
	}
	else
	{
		if (r != E_OPERATION_CANCELED)
		{
			ShowMessageBox(L"Scan", "Scan Failed or No Devices Found");
		}
		else
		{
			ShowMessageBox(L"Scan", "Scan Canceled");
		}
	}
}

void
MainForm::OnWifiDeactivated(result r)
{
	if (__isWifiDeactivating)
	{
		__isWifiDeactivating = false;
		result r = __pWifiDirectLocalDevice->Activate();

		if (r != E_SUCCESS)
		{
			HideWaitPopup();
			static_cast<CheckButton*>(__controlList.GetAt(ButtonActionType::ON_OFF))->SetSelected(false);
			ShowMessageBox(L"Activate", L"Failed to Activate");
		}
	}
}

void
MainForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    TryReturnVoid(pArgs != null, "The argument list is null");
    unique_ptr<IList> pList(pArgs);

    if (previousSceneId == SCENE_GROUP_CONFIG_FORM)
    {
        WifiDirectGroupInfo* pGroupInfo = static_cast<WifiDirectGroupInfo*>(pList->GetAt(0));
        AppAssert(__pWifiDirectLocalDevice);
        __pWifiDirectLocalDevice->SetGroupSettingInfo(*pGroupInfo);
    }
    else if(previousSceneId == SCENE_WPS_CONFIG_MODE_FORM)
    {
        String* pString = static_cast<String*>(pList->GetAt(0));
        WifiWpsConfigurationMode wpsConfigurationMode;
        Integer::Decode(*pString, (int&)wpsConfigurationMode);
        AppAssert(__pWifiDirectLocalDevice);
        __pWifiDirectLocalDevice->SetWpsConfigurationModePreference(wpsConfigurationMode);
    }
    else if(previousSceneId == SCENE_SCAN_RESULT_DEVICE_LIST_FORM)
    {
        WifiDirectDeviceInfo* pDeviceInfo = static_cast<WifiDirectDeviceInfo*>(pList->GetAt(0));
        this->Connect(*pDeviceInfo);
    }
    else if(previousSceneId == SCENE_PEER_DEVICE_LIST_FORM)
    {
        WifiDirectDeviceInfo* pDeviceInfo = static_cast<WifiDirectDeviceInfo*>(pList->GetAt(0));
        this->Disconnect(*pDeviceInfo);
    }

    RequestRedraw();
}


void
MainForm::OnSceneDeactivated(const SceneId &currentSceneId, const SceneId &nextSceneId)
{
}
void
MainForm::UpdateControl(void)
{
    Footer* pFooter = GetFooter();

    if (pFooter->GetItemCount() != 0)
    {
        pFooter->RemoveAllItems();
    }

    FooterItem footerItemDeviceInfo;
    footerItemDeviceInfo.Construct(ID_FOOTER_DEVICE_INFO);
    footerItemDeviceInfo.SetText("Local Device Info");
    pFooter->AddItem(footerItemDeviceInfo);

    switch(__currentState)
    {
    case STATE_DEACTIVATED:
        {
        	__pEditFieldDeviceName->SetEnabled(true);
            static_cast<CheckButton*>(__controlList.GetAt(ButtonActionType::ON_OFF))->SetSelected(false);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::SCAN_CONNECT))->SetEnabled(false);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::CREATE_AUTO_GROUP))->SetEnabled(false);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::LEAVE_GROUP))->SetEnabled(false);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::START_CHAT))->SetEnabled(false);
        }
        break;
    case STATE_ACTIVATED:
        {
        	__pEditFieldDeviceName->SetEnabled(false);
            static_cast<CheckButton*>(__controlList.GetAt(ButtonActionType::ON_OFF))->SetSelected(true);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::SCAN_CONNECT))->SetEnabled(true);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::CREATE_AUTO_GROUP))->SetEnabled(true);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::LEAVE_GROUP))->SetEnabled(false);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::START_CHAT))->SetEnabled(false);

            FooterItem footerItemGroupConfig;
            footerItemGroupConfig.Construct(ID_FOOTER_GROUP_CONFIG);
            footerItemGroupConfig.SetText("Group Configuration");
            pFooter->AddItem(footerItemGroupConfig);

            FooterItem footerItemWpsConfig;
            footerItemWpsConfig.Construct(ID_FOOTER_WPS_CONFIG);
            footerItemWpsConfig.SetText("WPS Configuration");
            pFooter->AddItem(footerItemWpsConfig);
        }
        break;
    case STATE_AUTO_GROUP_OWNER:
           {
           	   __pEditFieldDeviceName->SetEnabled(false);
               static_cast<CheckButton*>(__controlList.GetAt(ButtonActionType::ON_OFF))->SetSelected(true);
               static_cast<Button*>(__controlList.GetAt(ButtonActionType::SCAN_CONNECT))->SetEnabled(true);
               static_cast<Button*>(__controlList.GetAt(ButtonActionType::CREATE_AUTO_GROUP))->SetEnabled(false);
               static_cast<Button*>(__controlList.GetAt(ButtonActionType::LEAVE_GROUP))->SetEnabled(true);
               static_cast<Button*>(__controlList.GetAt(ButtonActionType::START_CHAT))->SetEnabled(false);

           }
           break;
    case STATE_GROUP_OWNER:
        {
        	__pEditFieldDeviceName->SetEnabled(false);
            static_cast<CheckButton*>(__controlList.GetAt(ButtonActionType::ON_OFF))->SetSelected(true);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::SCAN_CONNECT))->SetEnabled(true);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::CREATE_AUTO_GROUP))->SetEnabled(false);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::LEAVE_GROUP))->SetEnabled(true);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::START_CHAT))->SetEnabled(true);

            FooterItem footerItemPortonfig;
            footerItemPortonfig.Construct(ID_FOOTER_CHAT_PORT_CONFIG);
            footerItemPortonfig.SetText("Chat Port Configuration");
            pFooter->AddItem(footerItemPortonfig);

            FooterItem footerItemPeerList;
            footerItemPeerList.Construct(ID_FOOTER_PEER_DEVICE_LIST);
            footerItemPeerList.SetText("Peer List");
			pFooter->AddItem(footerItemPeerList);
        }
        break;
    case STATE_GROUP_CLIENT:
        {
        	__pEditFieldDeviceName->SetEnabled(false);
            static_cast<CheckButton*>(__controlList.GetAt(ButtonActionType::ON_OFF))->SetSelected(true);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::SCAN_CONNECT))->SetEnabled(false);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::CREATE_AUTO_GROUP))->SetEnabled(false);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::LEAVE_GROUP))->SetEnabled(true);
            static_cast<Button*>(__controlList.GetAt(ButtonActionType::START_CHAT))->SetEnabled(true);

            FooterItem footerItemPortonfig;
            footerItemPortonfig.Construct(ID_FOOTER_CHAT_PORT_CONFIG);
            footerItemPortonfig.SetText("Chat Port Configuration");
            pFooter->AddItem(footerItemPortonfig);

            FooterItem footerItemPeerList;
            footerItemPeerList.Construct(ID_FOOTER_PEER_DEVICE_LIST);
            footerItemPeerList.SetText("Peer List");
            pFooter->AddItem(footerItemPeerList);
        }
        break;
    default:
        break;
    }
    Draw(true);
}

bool
MainForm::UpdateCurrentState(void)
{
    TryReturn(__pWifiDirectLocalDevice != null, false, "Failed to get Wifi Direct Device");

    unique_ptr<WifiDirectDeviceInfo> pLocalDeviceInfo(__pWifiDirectLocalDevice->GetLocalDeviceInfoN());

    TryReturn(pLocalDeviceInfo != null, false, "Failed to get the information of local device");


    if (pLocalDeviceInfo->GetDeviceStatus() == WIFI_DIRECT_DEVICE_DEACTIVATED)
    {
        __currentState = STATE_DEACTIVATED;
    }
    else
    {
        __currentState = STATE_ACTIVATED;
    }

    if (pLocalDeviceInfo->GetGroupMemberType() == WIFI_DIRECT_GROUP_MEMBER_TYPE_OWNER)
    {
        __currentState = STATE_GROUP_OWNER;
    }
    else if (pLocalDeviceInfo->GetGroupMemberType() == WIFI_DIRECT_GROUP_MEMBER_TYPE_CLIENT)
    {
        __currentState = STATE_GROUP_CLIENT;
    }

    return true;
}

bool
MainForm::AddMainControl(ControlType type, const String& text, int firstActionId /*= -1*/, int secondActionId /*= -1*/)
{
    static const int ACTION_BUTTON_HEIGHT = 74;
    static const int EDIT_FIELD_HEIGHT = 112;
    static const int CHECK_BUTTON_HEIGHT = 112;
    static const int DEVICE_NAME_LIMIT = 64;
    static const int LEFT_MARGIN = 20;
    static const int RIGHT_MARGIN = 40;
    static const int DISPLAY_BUTTON_COUNT = ButtonActionType::COUNT - 1;
    static Rectangle clientRect = GetClientAreaBounds();
    //static int emptySpaceHeight = (clientRect.height - ((DISPLAY_BUTTON_COUNT * ACTION_BUTTON_HEIGHT) + EDIT_FIELD_HEIGHT + CHECK_BUTTON_HEIGHT)) / (DISPLAY_BUTTON_COUNT + 1); // button + edit filed
    static int emptySpaceHeight = 10;
    static int currentHeight = 0;


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
    else if (type == CONTROL_BUTTON)
    {
        Button* pButton = new Button();
        pButton->Construct(Rectangle(LEFT_MARGIN, currentHeight, clientRect.width - RIGHT_MARGIN, ACTION_BUTTON_HEIGHT), text);
        pButton->SetActionId(firstActionId);
        pButton->AddActionEventListener(*this);
        AddControl(*pButton);
        __controlList.Add(pButton);

        currentHeight += ACTION_BUTTON_HEIGHT + emptySpaceHeight;
    }
    else if (type == CONTROL_CHECK_BUTTON_STYLE_RADIO)
    {
        CheckButton* pCheckButton = new CheckButton();
        pCheckButton->Construct(Rectangle(LEFT_MARGIN, currentHeight, clientRect.width - RIGHT_MARGIN, CHECK_BUTTON_HEIGHT), CHECK_BUTTON_STYLE_ONOFF_SLIDING, BACKGROUND_STYLE_DEFAULT, false, text);
        pCheckButton->SetActionId(firstActionId, secondActionId);
        pCheckButton->AddActionEventListener(*this);
        AddControl(*pCheckButton);
        __controlList.Add(pCheckButton);

        currentHeight += CHECK_BUTTON_HEIGHT + emptySpaceHeight;
    }

    return true;
}

bool
MainForm::AddPopup(void)
{
	static const int POPUP_BUTTON_WIDTH = 150;
	static const int POPUP_BUTTON_HEIGHT = 75;
	static const int POPUP_BUTTON_Y = 50;
	static const int POPUP_MARGIN = 30;

	__pPopupWait = new Popup();
	__pPopupWait->Construct(true, Dimension(GetClientAreaBounds().width - POPUP_MARGIN*2, GetClientAreaBounds().height / 5));
	__pPopupWait->SetTitleText("Information");

	Rectangle rect;
	rect = __pPopupWait->GetClientAreaBounds();
	__pLabel = new (std::nothrow) Label();
	__pLabel->Construct(Rectangle(0,0,rect.width, rect.height), L"");
	__pLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	__pLabel->SetTextConfig(25, LABEL_TEXT_STYLE_NORMAL);
	__pPopupWait->AddControl(__pLabel);

	__pPopupScan = new Popup();
	__pPopupScan->Construct(true, Dimension(GetClientAreaBounds().width, GetClientAreaBounds().height / 3));
	__pPopupScan->SetTitleText(L"Scanning...");

	Button* pButtonCancelScan = new Button();
	pButtonCancelScan->Construct(Rectangle(((__pPopupScan->GetClientAreaBounds().width / 2) - POPUP_BUTTON_HEIGHT), POPUP_BUTTON_Y, POPUP_BUTTON_WIDTH, POPUP_BUTTON_HEIGHT), L"Cancel");
	pButtonCancelScan->SetActionId(ID_BUTTON_CANCEL_SCAN);
	pButtonCancelScan->AddActionEventListener(*this);
	__pPopupScan->AddControl(*pButtonCancelScan);

	__pPopupConnect = new Popup();
	__pPopupConnect->Construct(true, Dimension(GetClientAreaBounds().width, GetClientAreaBounds().height / 3));
	__pPopupConnect->SetTitleText(L"Connecting...");

	Button* pButtonCancelConnect = new Button();
	pButtonCancelConnect->Construct(Rectangle(((__pPopupConnect->GetClientAreaBounds().width / 2) - POPUP_BUTTON_HEIGHT), POPUP_BUTTON_Y, POPUP_BUTTON_WIDTH, POPUP_BUTTON_HEIGHT), L"Cancel");
	pButtonCancelConnect->SetActionId(ID_BUTTON_CANCEL_CONNECT);
	pButtonCancelConnect->AddActionEventListener(*this);
	__pPopupConnect->AddControl(*pButtonCancelConnect);

	return true;
}

bool
MainForm::UpdateLocalDeviceName(const String& deviceName /*= L""*/)
{
    TryReturn(__pWifiDirectLocalDevice != null, false, "Failed to get Wifi Direct Device");

    if (deviceName.IsEmpty())
    {
    	unique_ptr<WifiDirectDeviceInfo> pLocalDeviceInfo(__pWifiDirectLocalDevice->GetLocalDeviceInfoN());

		TryReturn(pLocalDeviceInfo != null, false, "Failed to get the information of local device");

		String deviceName = pLocalDeviceInfo->GetDeviceName();
		__pEditFieldDeviceName->SetText(deviceName);

    }
    else
    {
    	__pWifiDirectLocalDevice->SetLocalDeviceName(deviceName);
    }
    __pEditFieldDeviceName->Draw();
    return true;
}

result
MainForm::Activate(void)
{
	result r = __pWifiDirectLocalDevice->Activate();

	if (r == E_SUCCESS)
	{
		ShowWaitPopup(L"Activating...");
	}
	else
	{
		static_cast<CheckButton*>(__controlList.GetAt(ButtonActionType::ON_OFF))->SetSelected(false);
		ShowMessageBox(L"Activate", L"Failed to Activate");
	}

	return r;
}

result
MainForm::Scan(void)
{
    result r = __pWifiDirectLocalDevice->Scan();
    if (r == E_SUCCESS)
    {
        ShowScanPopup();
    }
    else
    {
        ShowMessageBox(L"Scan", L"Failed to Start Scan");
    }
    return r;
}

result
MainForm::Connect(const WifiDirectDeviceInfo& remoteDeviceInfo)
{

    result r = __pWifiDirectLocalDevice->Connect(remoteDeviceInfo);

    if (r == E_SUCCESS)
    {
        ShowConnectPopup();
    }
    else
    {
        ShowMessageBox(L"Connect", L"Failed to Connect");
    }
    return r;
}
result
MainForm::Disconnect(const WifiDirectDeviceInfo& peerDeviceInfo)
{
    result r = __pWifiDirectLocalDevice->Disconnect(peerDeviceInfo);

    if (r == E_SUCCESS)
    {
        ShowWaitPopup(L"Disconnecting...");
    }
    else
    {
        ShowMessageBox(L"Disconnect", L"Failed to Disconnect");
    }
    return r;
}
