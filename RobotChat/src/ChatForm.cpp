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

#include <unique_ptr.h>
#include "ChatForm.h"
#include "MainForm.h"
#include "SceneRegister.h"
#include "FormFactory.h"

using namespace std;
using namespace Tizen::Base;
using namespace Tizen::Net;
using namespace Tizen::Net::Wifi;
using namespace Tizen::Net::Sockets;
using namespace Tizen::App;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base::Utility;
using namespace Tizen::Ui::Scenes;

ChatTab::ChatTab(void)
	: __privateChatEditField()
	, __privateChatEditArea()
	, __deviceAddress()
	, __tabActionId(-1)
{
}

ChatTab::ChatTab(const String& deviceAddress, int tabActionId)
	: __privateChatEditField()
	, __privateChatEditArea()
	, __deviceAddress(deviceAddress)
	, __tabActionId(tabActionId)
{
}

ChatTab::~ChatTab(void)
{
}

void
ChatTab::OnIncomingMessage(const String& message)
{
	__privateChatEditArea.Append(message);
	__privateChatEditArea.Append(L"\n");
}

void
ChatTab::SavePrivateChat(const String& editArea, const String& editField)
{
	__privateChatEditArea.Clear();
	__privateChatEditArea.Append(editArea);
	__privateChatEditField.Clear();
	__privateChatEditField.Append(editField);
}

void
ChatTab::SavePrivateChatEditField(const String& editField)
{
	__privateChatEditField.Clear();
	__privateChatEditField.Append(editField);
}

void
ChatTab::SavePrivateChatEditArea(const String& editArea)
{
	__privateChatEditArea.Clear();
	__privateChatEditArea.Append(editArea);
}

String
ChatTab::GetDeviceAddress(void) const
{
	return __deviceAddress;
}

String
ChatTab::GetPrivateChatArea(void) const
{
	return __privateChatEditArea;
}

String
ChatTab::GetPrivateChatField(void) const
{
	return __privateChatEditField;
}

bool
ChatTab::operator ==(const ChatTab& rhs) const
{
	return (__privateChatEditField == rhs.__privateChatEditField &&
			__privateChatEditArea == rhs.__privateChatEditArea &&
			__deviceAddress == rhs.__deviceAddress &&
			__tabActionId == rhs.__tabActionId);
}

bool
ChatTab::operator !=(const ChatTab& rhs) const
{
	return !(*this == rhs);
}

ChatForm::ChatForm(void)
	: __localIpAddress()
	, __localDeviceName()
	, __broadcastMessageHeader(L"WifiDirectBroadcast")
	, __pTabBarChat(null)
	, __pEditAreaBroadcastChat(null)
	, __pEditAreaPrivateChat(null)
	, __pEditFieldBroadcastChat(null)
	, __pEditFieldPrivateChat(null)
	, __pPopupRequestingInfo(null)
	, __pPopupInitializingChat(null)
	, __pLabelInitializingChat(null)
	, __wifiDirectGroupMemberType(WIFI_DIRECT_GROUP_MEMBER_TYPE_NONE)
	, __pWifiDirectDevice(null)
	, __pNetConnection(null)
	, __pUdpSocket(null)
	, __lastTabIndex(-1)
	, __lastTabActionId(-1)
	, __keypadActionId(-1)
	, __chatPortNumber(0)
	, __isKeypadEnabled(false)
{
}

ChatForm::~ChatForm(void)
{
	delete __pPopupRequestingInfo;
	delete __pPopupInitializingChat;
	delete __pUdpSocket;
	delete __pNetConnection;
}

bool
ChatForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL | FORM_STYLE_HEADER | FORM_STYLE_INDICATOR | FORM_STYLE_FOOTER);
	SetName(FORM_CHAT);

	NetAccountManager netAccountManager;
    NetAccountId accountId = INVALID_HANDLE;

	netAccountManager.Construct();
	accountId = netAccountManager.GetNetAccountId(NET_BEARER_WIFI_DIRECT);
	TryReturn(accountId != INVALID_HANDLE, false, "Failed to get net account Id");

	result r = E_SUCCESS;

	__pNetConnection = new NetConnection();

	r = __pNetConnection->Construct(accountId);
	TryReturn(r == E_SUCCESS, false, "Failed to construct NetConnection");

	r = __pNetConnection->AddNetConnectionListener(*(INetConnectionEventListener*) this);
	TryReturn(r == E_SUCCESS, false, "Failed to add listener to NetConnection");

	r = __pNetConnection->Start();
	TryReturn(r == E_SUCCESS, false, "Failed to start NetConnection");

	return true;
}

void
ChatForm::InitiatePrivateChat(const String& deviceName, const String& ipAddress)
{
	int newActionId = 0;
	int tabIndex = 0;
	ChatTab privateChat;
	int count = __privateChatList.GetCount();
	int i = 0;

	if (ipAddress.Equals(__localIpAddress))
	{
		ShowMessageBox(L"Private Chat", L"Selected Device is own Device");
		return;
	}

	for ( ; i < count; i++)
	{
		__privateChatList.GetAt(i, privateChat);
		if (ipAddress.CompareTo(privateChat.GetDeviceAddress()) == 0)
		{
			break;
		}
	}

	if (i < count)
	{
		AppLog("Private Chat already exists at index %d", i + 1);

		// Shifts the focus on this tab.
		tabIndex = i + 1;
		SwitchPrivateChatTab(__pTabBarChat->GetSelectedItemIndex(), tabIndex);

		__pTabBarChat->SetItemSelected(tabIndex);
		RequestRedraw();

		__lastTabActionId = GetItemActionIdAt(tabIndex);
	}
	else
	{
		AppLog("New Chat Tab created.");
		newActionId = GetActionId();

		if (newActionId < 0)
		{
			AppLog("Too many tabs open");
			ShowMessageBox(L"Initiate New Chat", L"Too Many tabs open\nPlease close some tabs");
			return;
		}

		ChatTab* pNewPrivateChat = new ChatTab(ipAddress, newActionId);
		if (!pNewPrivateChat)
		{
			ShowMessageBox(L"New Chat", L"Failed To Initiate New Chat");
			return;
		}
		__privateChatList.Add(*pNewPrivateChat);
		delete pNewPrivateChat;

		if (__pTabBarChat)
		{
			TabBarItem tabBarItem;
			tabBarItem.Construct(deviceName, newActionId);
			result r = __pTabBarChat->AddItem(tabBarItem);

			if (r != E_SUCCESS)
			{
				FreeActionId(newActionId);
				AppLog("Creating New Tab Failed. Error [%s]", GetErrorMessage(r));
				return;
			}
		}
		__tabIndexArray[__pTabBarChat->GetItemCount() - 2] = newActionId;

		tabIndex = GetItemIndexFromActionId(newActionId);

		SwitchPrivateChatTab(__pTabBarChat->GetSelectedItemIndex(), tabIndex);

		__pTabBarChat->SetItemSelected(tabIndex);

		__lastTabActionId = newActionId;
	}

	UpdateFooterControl();

	RequestRedraw();
	__lastTabIndex = tabIndex;
}

void
ChatForm::FinishPrivateChat(void)
{
	int currIndex = __pTabBarChat->GetSelectedItemIndex();
	int actionId = 0;

	if (currIndex != 0)
	{
		actionId = GetItemActionIdAt(currIndex);
		__pTabBarChat->RemoveItemAt(currIndex);
		__privateChatList.RemoveAt(currIndex - 1);
		FreeActionId(actionId);
		__pEditAreaPrivateChat->Clear();
		__pEditFieldPrivateChat->Clear();
		__lastTabActionId = actionId;
		__pTabBarChat->SetItemSelected(0);
		RemoveItemActionId(currIndex);
		SwitchToBroadcastTab();
	}
	else
	{
		AppLog("Invalid Scenario, index [%d]", currIndex);
	}
}

void
ChatForm::SwitchPrivateChatTab(int oldTabIndex, int tabIndex)
{
	AppLog("Index of the selected tab [%d], current tab[%d]", tabIndex, oldTabIndex);
	TryReturnVoid(tabIndex != oldTabIndex, "The tab index is equal to old tab index");

	ChatTab ChatTab;
	int deviceIndex = 0;
	__pTabBarChat->SetItemSelected(tabIndex);
	if (oldTabIndex > 0)
	{
		deviceIndex = oldTabIndex - 1;
		__privateChatList.GetAt(deviceIndex, ChatTab);
		if (__isKeypadEnabled == false)
		{
			ChatTab.SavePrivateChat(__pEditAreaPrivateChat->GetText(), __pEditFieldPrivateChat->GetText());
		}
		else
		{
			ChatTab.SavePrivateChatEditArea(__pEditAreaPrivateChat->GetText());
		}
		__privateChatList.SetAt(ChatTab, deviceIndex);
	}

	deviceIndex = tabIndex - 1;
	__privateChatList.GetAt(deviceIndex, ChatTab);

	Header* pHeader = GetHeader();
	if (pHeader != null)
	{
		pHeader->SetTitleText(ChatTab.GetDeviceAddress());
	}
	__pEditAreaPrivateChat->Clear();
	__pEditAreaPrivateChat->SetText(ChatTab.GetPrivateChatArea());

	if (__isKeypadEnabled == false)
	{
		AppLog("keypad is not visible");
		__pEditFieldPrivateChat->Clear();
		__pEditFieldPrivateChat->SetText(ChatTab.GetPrivateChatField());
	}
	else
	{
		AppLog("keypad is visible");
	}

	__pEditAreaBroadcastChat->SetShowState(false);
	__pEditAreaPrivateChat->SetShowState(true);

	__pEditFieldBroadcastChat->SetShowState(false);
	__pEditFieldPrivateChat->SetShowState(true);


	__lastTabIndex = tabIndex;
	__lastTabActionId = GetItemActionIdAt(tabIndex);

	UpdateFooterControl();
	SetFocus();
	Draw(true);
}

void
ChatForm::SwitchToBroadcastTab(void)
{
	ChatTab ChatTab;
	int tempIndex = 0;

	if (__lastTabIndex != 0) // 0 is the default Index for BroadCast Tab
	{
	    UpdateFooterControl();

		if (IsFreeActionId(__lastTabActionId) != true)
		{
			tempIndex = GetItemIndexFromActionId(__lastTabActionId);
			if (tempIndex != -1)
			{
				AppLog("Updating Private Chat Previous Tab index is %d ", tempIndex);
				__privateChatList.GetAt(tempIndex - 1, ChatTab);
				if (__isKeypadEnabled == false)
				{
					ChatTab.SavePrivateChat(__pEditAreaPrivateChat->GetText(), __pEditFieldPrivateChat->GetText());
				}
				else
				{
					ChatTab.SavePrivateChatEditArea(__pEditAreaPrivateChat->GetText());
				}
				__privateChatList.SetAt(ChatTab, tempIndex - 1);

				__lastTabIndex = tempIndex;
			}
			else
			{
				AppLog("Previous tab was Deleted");
			}
		}

		// Saves the content of the previous private chat tab.
		__lastTabIndex = 0;
		__lastTabActionId = ID_TAB_BROADCAST;

		Header* pHeader = GetHeader();
		if (pHeader != null)
		{
			pHeader->SetTitleText(__localIpAddress);
		}
		__pEditAreaPrivateChat->SetShowState(false);
		__pEditAreaBroadcastChat->SetShowState(true);

		__pEditFieldPrivateChat->SetShowState(false);
		__pEditFieldBroadcastChat->SetShowState(true);

		Draw(true);
	}
	else
	{
		AppLog("Broadcast Switch fail because previous tab index [%d]", __lastTabIndex);
	}
	SetFocus();
}

void
ChatForm::SendPrivateChatMessage(void)
{
	result r = E_SUCCESS;
	int deviceIndex = __pTabBarChat->GetSelectedItemIndex();

	TryReturnVoid(__pEditFieldPrivateChat->GetTextLength() != 0, "Private Chat field empty");

	String privateMessage(__pEditFieldPrivateChat->GetText());
	String deviceNameLength;
	ByteBuffer* pTxBuffer = null;

	deviceNameLength.Format(3, L"%2d", __localDeviceName.GetLength());
	privateMessage.Insert(L":", 0);
	privateMessage.Insert(__localDeviceName, 0);
	privateMessage.Insert(deviceNameLength, 0);

	ChatTab privateChatTab;
	__privateChatList.GetAt(deviceIndex - 1, privateChatTab);

	Ip4Address ipAddress(privateChatTab.GetDeviceAddress());

	NetEndPoint recvEndPoint(ipAddress, __chatPortNumber);
	r = GetLastResult();
	TryReturnVoid(r == E_SUCCESS, "Failed to create NetEndPoint. Error [%s]", GetErrorMessage(r));

	pTxBuffer = StringUtil::StringToUtf8N(privateMessage);
	r = GetLastResult();
	if (r != E_SUCCESS)
	{
		AppLog("Failed to convert String. Error [%s]", GetErrorMessage(r));
		if (pTxBuffer != null)
		{
			delete pTxBuffer;
		}
		return;
	}

	r = __pUdpSocket->SendTo(*pTxBuffer, recvEndPoint);
	AppLog("Send Private Message. Result [%s]", GetErrorMessage(r));

	if (r == E_SUCCESS)
	{
		privateMessage.Remove(0, deviceNameLength.GetLength());
		privateMessage.Append("\n");

		__pEditFieldPrivateChat->Clear();
		__pEditAreaPrivateChat->AppendText(privateMessage);

		privateChatTab.SavePrivateChat(__pEditAreaPrivateChat->GetText(), __pEditFieldPrivateChat->GetText());
		__privateChatList.SetAt(privateChatTab, deviceIndex - 1);

		__pEditAreaPrivateChat->Draw();
		__pEditFieldPrivateChat->Draw();
	}

	delete pTxBuffer;
}

void
ChatForm::SendBroadcastChatMessage(void)
{
	TryReturnVoid(__pEditFieldBroadcastChat->GetTextLength() != 0, "Broadcast Chat field empty");

	result r = E_SUCCESS;
	String broadcastMessage(__pEditFieldBroadcastChat->GetText());
	Ip4Address broadcastAddress(L"255.255.255.255");
	ByteBuffer* pTxBuffer = null;

	broadcastMessage.Insert(L":", 0);
	broadcastMessage.Insert(__localDeviceName, 0);
	broadcastMessage.Insert(__broadcastMessageHeader, 0);

	NetEndPoint recvEndPoint(broadcastAddress, __chatPortNumber);
	r = GetLastResult();
	TryReturnVoid(r == E_SUCCESS, "Failed to create NetEndPoint. Error [%s]", GetErrorMessage(r));

	pTxBuffer = StringUtil::StringToUtf8N(broadcastMessage);
	r = GetLastResult();
	if (r != E_SUCCESS)
	{
		AppLog("Failed to convert String. Error [%s]", GetErrorMessage(r));
		delete pTxBuffer;
		return;
	}

	r = __pUdpSocket->SendTo(*pTxBuffer, recvEndPoint);
	AppLog("Send Broadcast Message. Result [%s]", GetErrorMessage(r));

	if (r == E_SUCCESS)
	{
		broadcastMessage.Remove(0, __broadcastMessageHeader.GetLength());
		broadcastMessage.Append("\n");

		__pEditFieldBroadcastChat->Clear();
		__pEditFieldBroadcastChat->RequestRedraw();

		__pEditAreaBroadcastChat->AppendText(broadcastMessage);
		__pEditAreaBroadcastChat->Draw();
	}

	delete pTxBuffer;
}

void
ChatForm::DisplayGroupMembers(const IList* pWifiDirectDeviceInfoList)
{
	__pPopupRequestingInfo->SetShowState(false);
	if (pWifiDirectDeviceInfoList != null)
	{
		SceneManager* pSceneManager = SceneManager::GetInstance();
		AppAssert(pSceneManager);

		pSceneManager->GoForward(ForwardSceneTransition(SCENE_CHAT_MEMBER_DEVICE_LIST_FORM), pWifiDirectDeviceInfoList);
	}
	else
	{
		ShowMessageBox(L"Chat Members", L"Failed To Get Group Member Info From Group Owner");
	}
}

void
ChatForm::ShowMessageBox(const String& title, const String& text)
{
	MessageBox messageBox;
	int modalResult = 0;

	messageBox.Construct(title, text, MSGBOX_STYLE_NONE, MESSAGE_BOX_TIMEOUT);
	messageBox.ShowAndWait(modalResult);
}

result
ChatForm::InitializeChatSocket(void)
{
	result r = E_SUCCESS;
	Ip4Address localAddr(L"0");  // IN_ADDR_ANY
	NetEndPoint localEndPoint(localAddr, __chatPortNumber);

	__pUdpSocket = new Socket();

	r = __pUdpSocket->Construct(*__pNetConnection, NET_SOCKET_AF_IPV4, NET_SOCKET_TYPE_DATAGRAM, NET_SOCKET_PROTOCOL_UDP);
	TryReturn(r == E_SUCCESS, r, "[%s] Sokcet Construct Failed", GetErrorMessage(r));

	r = __pUdpSocket->SetSockOpt(NET_SOCKET_SOL_SOCKET, NET_SOCKET_SO_BROADCAST, 1);
	TryReturn(r == E_SUCCESS, r, "[%s] SetSockOpt Failed", GetErrorMessage(r));

	r = __pUdpSocket->AddSocketListener(*(ISocketEventListener*) this);
	TryReturn(r == E_SUCCESS, r, "[%s] AddSocketListener Failed", GetErrorMessage(r));

	r = __pUdpSocket->AsyncSelectByListener(NET_SOCKET_EVENT_READ | NET_SOCKET_EVENT_WRITE | NET_SOCKET_EVENT_CLOSE);
	TryReturn(r == E_SUCCESS, r, "[%s] AsyncSelectByListener Failed", GetErrorMessage(r));

	r = __pUdpSocket->Bind(localEndPoint);
	TryReturn(r == E_SUCCESS, r, "[%s] UDP socket Bind Failed", GetErrorMessage(r));

	return r;
}

int
ChatForm::GetActionId(void)
{
	for (int i = 0; i < MAX_TAB_COUNT; i++)
	{
		if (__actionIdArray[i] == 0)
		{
			__actionIdArray[i] = 1;
			return (i + ID_TAB_BROADCAST + 1);
		}
	}
	return -1;
}

void
ChatForm::FreeActionId(int actionId)
{
	int arrIndex = actionId - ID_TAB_BROADCAST - 1;
	__actionIdArray[arrIndex] = 0;
}

bool
ChatForm::IsFreeActionId(int actionId)
{
	bool result = false;

	int arrIndex = actionId - ID_TAB_BROADCAST - 1;

	if (__actionIdArray[arrIndex] == 0)
	{
		result = true;
	}

	return result;
}

int
ChatForm::GetItemActionIdAt(int tabIndex)
{
	return (__tabIndexArray[tabIndex - 1]);
}

int
ChatForm::GetItemIndexFromActionId(int actionId)
{
	for (int i = 0; i < MAX_TAB_COUNT; i++)
	{
		if (actionId == __tabIndexArray[i])
		{
			return i + 1;
		}
	}

	return -1;
}

void
ChatForm::RemoveItemActionId(int tabIndex)
{
	for (int i = tabIndex; i < MAX_TAB_COUNT; i++)
	{
		__tabIndexArray[i - 1] = __tabIndexArray[i];
	}
}

result
ChatForm::OnInitializing(void)
{
	Header* pHeader = GetHeader();
	if (pHeader)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
	}

	SetFormBackEventListener(this);

	Rectangle clientRect = GetClientAreaBounds();
	int emptySpaceHeight = 0;
	int currentHeight = 0;

	const int indentation = 0;

	__pTabBarChat = new TabBar();
	__pTabBarChat->Construct(0, 0, GetClientAreaBounds().width);
	__pTabBarChat->AddActionEventListener(*this);
	AddControl(*__pTabBarChat);

	TabBarItem tabBarItemBroadcast;
	tabBarItemBroadcast.Construct(L"BROADCAST", ID_TAB_BROADCAST);
	__pTabBarChat->AddItem(tabBarItemBroadcast);

	emptySpaceHeight = (clientRect.height - (__pTabBarChat->GetBounds().height + EDIT_FIELD_HEIGHT + EDIT_AREA_HEIGHT)) / MAX_NO_CONTROL;
	currentHeight = __pTabBarChat->GetBounds().height + emptySpaceHeight;

	__pEditAreaBroadcastChat = new EditArea;
	__pEditAreaBroadcastChat->Construct(Rectangle(indentation, currentHeight, clientRect.width - indentation * 2, EDIT_AREA_HEIGHT));
	__pEditAreaBroadcastChat->SetKeypadEnabled(false);
	__pEditAreaBroadcastChat->SetViewModeEnabled(true);
	AddControl(*__pEditAreaBroadcastChat);

	__pEditAreaPrivateChat = new EditArea;
	__pEditAreaPrivateChat->Construct(Rectangle(indentation, currentHeight, clientRect.width - indentation * 2, EDIT_AREA_HEIGHT));
	__pEditAreaPrivateChat->SetKeypadEnabled(false);
	__pEditAreaPrivateChat->SetViewModeEnabled(true);
	AddControl(*__pEditAreaPrivateChat);
	__pEditAreaPrivateChat->SetShowState(false);

	currentHeight += EDIT_AREA_HEIGHT + emptySpaceHeight;

	__pEditFieldBroadcastChat = new EditField;
	__pEditFieldBroadcastChat->Construct(Rectangle(indentation, currentHeight, clientRect.width - indentation * 2, EDIT_FIELD_HEIGHT));
	AddControl(*__pEditFieldBroadcastChat);

	__pEditFieldPrivateChat = new EditField;
	__pEditFieldPrivateChat->Construct(Rectangle(indentation, currentHeight, clientRect.width - indentation * 2, EDIT_FIELD_HEIGHT));
	AddControl(*__pEditFieldPrivateChat);

	__pEditFieldPrivateChat->AddTouchEventListener(*this);
	__pEditFieldPrivateChat->AddTextEventListener(*this);
	__pEditFieldPrivateChat->SetShowState(false);

	const int DEFAULT_MARGIN = 20;
	__pPopupRequestingInfo = new Popup;
	__pPopupRequestingInfo->Construct(true, Dimension(clientRect.width - DEFAULT_MARGIN, clientRect.height / 3));
	__pPopupRequestingInfo->SetTitleText(L"Requesting..");
	__pPopupRequestingInfo->SetShowState(false);

	__pPopupInitializingChat = new Popup;
	__pPopupInitializingChat->Construct(true, Dimension(clientRect.width - DEFAULT_MARGIN, clientRect.height / 2));
	__pPopupInitializingChat->SetTitleText(L"Wifi direct chat");

	const int POPUP_HEIGHT = 100;
	__pLabelInitializingChat = new Label;
	__pLabelInitializingChat->Construct(Rectangle(0, 0, __pPopupInitializingChat->GetClientAreaBounds().width, POPUP_HEIGHT), L"Please Wait\nInitializing Chat");
	__pLabelInitializingChat->SetTextConfig(POPUP_TEXT_SIZE, LABEL_TEXT_STYLE_NORMAL);
	__pLabelInitializingChat->SetTextVerticalAlignment(ALIGNMENT_MIDDLE);
	__pLabelInitializingChat->SetTextHorizontalAlignment(ALIGNMENT_CENTER);
	__pPopupInitializingChat->AddControl(*__pLabelInitializingChat);

	__pPopupInitializingChat->SetShowState(true);
	__pPopupInitializingChat->Show();

	for (int i = 0; i < MAX_TAB_COUNT; i++)
	{
		__actionIdArray[i] = 0;
		__tabIndexArray[i] = -1;
	}

	UpdateFooterControl();

	Draw(true);

	return E_SUCCESS;
}

void
ChatForm::OnActionPerformed(const Control& source, int actionId)
{

	int currIndex = __pTabBarChat->GetSelectedItemIndex();

	switch (actionId)
	{
	case ID_OPTION_SEND:
		if (currIndex == 0)
		{
			SendBroadcastChatMessage();
		}
		else
		{
			SendPrivateChatMessage();
		}
		SetFocus();
		break;
	case ID_OPTION_CLEAR_CHAT:
		if (currIndex == 0)
		{
			__pEditAreaBroadcastChat->Clear();
			__pEditAreaBroadcastChat->RequestRedraw();
		}
		else
		{
			__pEditAreaPrivateChat->Clear();
			__pEditAreaPrivateChat->RequestRedraw();
		}
		break;

	case ID_OPTION_CHAT_MEMBERS:
		if (__wifiDirectGroupMemberType == WIFI_DIRECT_GROUP_MEMBER_TYPE_OWNER)
		{
			result r = E_FAILURE;
			IList* pWifiDirectDeviceInfoList = __pWifiDirectDevice->GetGroupClientInfoListN();
            if (pWifiDirectDeviceInfoList != null)
            {
                String* pString = new String(L"true");
                if (pString != null)
                {
                    r = E_SUCCESS;
                    pWifiDirectDeviceInfoList->InsertAt(*pString, 0);
                    DisplayGroupMembers(pWifiDirectDeviceInfoList);
                }
                else
                {
                    pWifiDirectDeviceInfoList->RemoveAll(true);
                    delete pWifiDirectDeviceInfoList;
                }
            }
			if (IsFailed(r))
			{
				ShowMessageBox(L"Chat Members", L"Failed To Get Chat Member Information");
			}
		}
		else
	    {
		    result r = E_FAILURE;

		    IList* pWifiDirectDeviceInfoList = new ArrayList();
		    WifiDirectDeviceInfo* pDeviceInfo = __pWifiDirectDevice->GetGroupOwnerInfoN();
		    pWifiDirectDeviceInfoList->Add(*pDeviceInfo);
		    String* pString = new String(L"false");
            if (pString != null)
            {
                r = E_SUCCESS;
                pWifiDirectDeviceInfoList->InsertAt(*pString, 0);
                DisplayGroupMembers(pWifiDirectDeviceInfoList);
            }
            else
            {
                pWifiDirectDeviceInfoList->RemoveAll(true);
                delete pWifiDirectDeviceInfoList;
            }

            if (IsFailed(r))
            {
                ShowMessageBox(L"Chat Member", L"Failed To Get Chat Member Information");
            }
	    }
		break;

	case ID_OPTION_END_CHAT:
		FinishPrivateChat();
		break;

	case ID_TAB_BROADCAST:
		SwitchToBroadcastTab();
		break;

	default:
		if ((actionId > ID_TAB_BROADCAST) && (actionId <= ID_TAB_BROADCAST + MAX_TAB))
		{
			SwitchPrivateChatTab(__lastTabIndex, currIndex);
			__lastTabActionId = actionId;
			__lastTabIndex = currIndex;
		}
		break;
	}
}

void
ChatForm::OnNetConnectionStarted(NetConnection& netConnection, result r)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	if (r == E_SUCCESS)
	{
		if (netConnection.GetNetConnectionInfo() && netConnection.GetNetConnectionInfo()->GetLocalAddress())
		{
			__localIpAddress = netConnection.GetNetConnectionInfo()->GetLocalAddress()->ToString();
		}
		Header* pHeader = GetHeader();
		if (pHeader != null)
		{
			pHeader->SetTitleText(__localIpAddress);
		}
		__pPopupInitializingChat->SetShowState(false);

		r = InitializeChatSocket();
		if (IsFailed(r))
		{
			AppLog("Failed to create socket. [%s]", GetErrorMessage(r));

			if (r == E_ALREADY_BOUND)
			{
				ShowMessageBox(L"Start Chat", L"Port Already in use");
			}

			pSceneManager->GoBackward(BackwardSceneTransition());
		}
		RequestRedraw();
	}
	else
	{
		AppLog("Net Connection Failed. [%s]", GetErrorMessage(r));
		pSceneManager->GoBackward(BackwardSceneTransition());
	}

}

void
ChatForm::OnNetConnectionStopped(NetConnection& netConnection, result r)
{
    if (__pUdpSocket)
    {
        __pUdpSocket->Close();
        __pUdpSocket->RemoveSocketListener(*(ISocketEventListener*) this);
        delete __pUdpSocket;
        __pUdpSocket = null;
    }

	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoForward(ForwardSceneTransition(SCENE_MAIN_FORM, SCENE_TRANSITION_ANIMATION_TYPE_NONE,
	                        SCENE_HISTORY_OPTION_NO_HISTORY, SCENE_DESTROY_OPTION_DESTROY));
}

void
ChatForm::OnSocketReadyToReceive(Socket& socket)
{
	result r = E_SUCCESS;
	String message;
	unsigned long arg = 0;
	Ip4Address ip4Address("0"); //ADDR_ANY
	String deviceName(L"");
	int deviceIndex = 0;
	int tabIndex = __pTabBarChat->GetSelectedItemIndex();

	r = socket.Ioctl(NET_SOCKET_FIONREAD, arg);
	TryReturnVoid(r == E_SUCCESS, "Socket ioctl for read is failed");

	ByteBuffer rxBuffer;
	NetEndPoint netEndPoint(ip4Address, 0);
	rxBuffer.Construct(arg);

	r = socket.ReceiveFrom(rxBuffer, netEndPoint);
	TryReturnVoid(r == E_SUCCESS, "ReceiveFrom failed");
	AppLog("ReceivedFrom Success");

	IpAddress* pIpAddress = netEndPoint.GetAddress();
	TryReturnVoid(pIpAddress->GetNetAddressFamily() == NET_AF_IPV4, "IpAddress is not IPV4");
	String ipAddress = pIpAddress->ToString();

	// Whether it is my address or not
	if (__pNetConnection != null)
	{
		const IpAddress* localAddress = __pNetConnection->GetNetConnectionInfo()->GetLocalAddress();
		// Skip my address
		if (localAddress->ToString() == ipAddress)
		{
			return;
		}
	}

	rxBuffer.Flip();
	r = StringUtil::Utf8ToString((char*) rxBuffer.GetPointer(), message);

	String tempString;
	message.SubString(0, __broadcastMessageHeader.GetLength(), tempString);
	if (tempString.Equals(__broadcastMessageHeader))
	{
		AppLog("Broadcast Message Received");
		tempString.Clear();
		tempString.Append(message);
		tempString.Remove(0, __broadcastMessageHeader.GetLength());

		__pEditAreaBroadcastChat->AppendText(tempString);
		__pEditAreaBroadcastChat->AppendText("\n");

		if (tabIndex != 0)
		{
			__pTabBarChat->SetItemSelected(0);
			SwitchToBroadcastTab();
		}
		else
		{
			__pEditAreaBroadcastChat->Draw();
		}
	}
	else
	{
		AppLog("Private chat Message Received");

		ChatTab privateChatTab;

		if (tabIndex != 0)
		{
			deviceIndex = tabIndex - 1;

			__privateChatList.GetAt(deviceIndex, privateChatTab);

			if (ipAddress.CompareTo(privateChatTab.GetDeviceAddress()) == 0)
			{
				message.Remove(0, 2);
				__pEditAreaPrivateChat->AppendText(message);
				__pEditAreaPrivateChat->AppendText("\n");
				__pEditAreaPrivateChat->Draw();
				privateChatTab.SavePrivateChat(__pEditAreaPrivateChat->GetText(), __pEditAreaPrivateChat->GetText());
				__privateChatList.SetAt(privateChatTab, deviceIndex);
				return;
			}
		}

		int count = __privateChatList.GetCount();
		int newActionId = 0;
		int i = 0;

		for ( ; i < count; i++)
		{
			__privateChatList.GetAt(i, privateChatTab);

			if (ipAddress.CompareTo(privateChatTab.GetDeviceAddress()) == 0)
			{
				break;
			}
		}
		if (i < count)
		{
			AppLog("Private Chat already exits with this device at index %d", i + 1);
			message.Remove(0, 2);
			privateChatTab.OnIncomingMessage(message);

			__privateChatList.SetAt(privateChatTab, i);
			tabIndex = i + 1;
			SwitchPrivateChatTab(__pTabBarChat->GetSelectedItemIndex(), tabIndex);
			__pTabBarChat->SetItemSelected(tabIndex);

			RequestRedraw();

			__lastTabActionId = GetItemActionIdAt(tabIndex);
		}
		else
		{
			newActionId = GetActionId();
			if (newActionId < 0)
			{
				AppLog("Too many tabs open");
				ShowMessageBox(L"New Incoming Chat", L"Too Many tabs open\nPlease close some tabs");
				return;
			}

			// get the device name from the chat message
			int deviceNameLength = 0;
			message.SubString(0, 2, deviceName);
			deviceName.Trim();
			Integer::Decode(deviceName, deviceNameLength);
			deviceName.Clear();
			message.Remove(0, 2);
			message.SubString(0, deviceNameLength, deviceName);

			TabBarItem tabBarItem;
			tabBarItem.Construct(deviceName, newActionId);
			r = __pTabBarChat->AddItem(tabBarItem);
			if (r != E_SUCCESS)
			{
				FreeActionId(newActionId);
				AppLog("Creating New Tab Failed. Error [%s]", GetErrorMessage(r));
				return;
			}

			__tabIndexArray[__pTabBarChat->GetItemCount() - 2] = newActionId;

			ChatTab newPrivateChat(ipAddress, newActionId);

			tabIndex = GetItemIndexFromActionId(newActionId);

			newPrivateChat.OnIncomingMessage(message); // New Tab will be created but will be put in background.
			__privateChatList.Add(newPrivateChat);
			SwitchPrivateChatTab(__pTabBarChat->GetSelectedItemIndex(), tabIndex);

			RequestRedraw();
			__lastTabActionId = newActionId;
		}

		__lastTabIndex = tabIndex;
	}
}

void
ChatForm::OnTextValueChangeCanceled(const Control& source)
{
	int tempIndex = GetItemIndexFromActionId(__keypadActionId);

	if (tempIndex != -1 && tempIndex != __lastTabIndex)
	{
		ChatTab ChatTab;

		__privateChatList.GetAt(__lastTabIndex - 1, ChatTab);
		__pEditFieldPrivateChat->Clear();
		__pEditFieldPrivateChat->SetText(ChatTab.GetPrivateChatField());
		__keypadActionId = -1;
	}

	__isKeypadEnabled = false;
}

void
ChatForm::OnTextValueChanged(const Control& source)
{
	int tempIndex = GetItemIndexFromActionId(__keypadActionId);

	if (tempIndex != -1 && tempIndex != __lastTabIndex)
	{
		ChatTab ChatTab;

		__privateChatList.GetAt(tempIndex - 1, ChatTab);
		ChatTab.SavePrivateChatEditField(__pEditFieldPrivateChat->GetText());
		__privateChatList.SetAt(ChatTab, tempIndex - 1);

		__privateChatList.GetAt(__lastTabIndex - 1, ChatTab);
		__pEditFieldPrivateChat->Clear();
		__pEditFieldPrivateChat->SetText(ChatTab.GetPrivateChatField());
		__keypadActionId = -1;
	}

	SetFocus();
	__isKeypadEnabled = false;
}

void
ChatForm::OnTouchReleased(const Control& source, const Point& currentPosition, const TouchEventInfo& touchInfo)
{
	if ((source.Equals(*__pEditFieldPrivateChat)) && (__isKeypadEnabled == false))
	{
		__keypadActionId = __lastTabActionId;
		__isKeypadEnabled = true;
	}
}

void
ChatForm::OnFormBackRequested(Form& source)
{
	if (__pNetConnection != null)
	{
		if (__pNetConnection->Stop() == E_SUCCESS)
		{
			__pLabelInitializingChat->SetText(L"Please Wait\nStopping");
			__pPopupInitializingChat->SetShowState(true);
			__pPopupInitializingChat->RequestRedraw(true);
		}
		else
		{
			AppLog("Failed to Stop NetConnection");

			SceneManager* pSceneManager = SceneManager::GetInstance();
			AppAssert(pSceneManager);

			pSceneManager->GoBackward(BackwardSceneTransition());
		}
	}
}

void
ChatForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    TryReturnVoid(pArgs != null, "The argument list is null");

    __pWifiDirectDevice = static_cast<WifiDirectDevice*>(pArgs->GetAt(0));

    String* pPortNumber = static_cast<String*>(pArgs->GetAt(1));
    Integer::Decode(*pPortNumber, __chatPortNumber);

    delete pArgs;

    WifiDirectDeviceInfo* pWifiDirectDeviceInfo = __pWifiDirectDevice->GetLocalDeviceInfoN();
    TryReturnVoid(pWifiDirectDeviceInfo != null, "Failed to get Device Info");

    __localDeviceName.Append(pWifiDirectDeviceInfo->GetDeviceName());
    __wifiDirectGroupMemberType = pWifiDirectDeviceInfo->GetGroupMemberType();

    delete pWifiDirectDeviceInfo;

    RequestRedraw();
}

void
ChatForm::UpdateFooterControl(void)
{
    Footer* pFooter = GetFooter();

    if (pFooter->GetItemCount() != 0)
    {
        pFooter->RemoveAllItems();
    }

    pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);

    FooterItem footerItemSend;
    footerItemSend.Construct(ID_OPTION_SEND);
    footerItemSend.SetText("Send");
    pFooter->AddItem(footerItemSend);

    FooterItem footerItemClear;
    footerItemClear.Construct(ID_OPTION_CLEAR_CHAT);
    footerItemClear.SetText("Clear Chat");
    pFooter->AddItem(footerItemClear);

    FooterItem footerItemChatMembers;
    footerItemChatMembers.Construct(ID_OPTION_CHAT_MEMBERS);
    footerItemChatMembers.SetText("Chat Members");
    pFooter->AddItem(footerItemChatMembers);


    int currIndex = __pTabBarChat->GetSelectedItemIndex();

    if (currIndex != 0)
    {
        Footer* pFooter = GetFooter();
        FooterItem footerItemEndChat;
        footerItemEndChat.Construct(ID_OPTION_END_CHAT);
        footerItemEndChat.SetText("End Chat");
        pFooter->AddItem(footerItemEndChat);

    }

    pFooter->AddActionEventListener(*this);
}
