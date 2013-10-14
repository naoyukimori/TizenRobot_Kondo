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

#ifndef _CHAT_FORM_H_
#define _CHAT_FORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FNet.h>

class ChatTab
{
public:
	ChatTab(void);
	ChatTab(const Tizen::Base::String& deviceAddress, int tabActionId);
	~ChatTab(void);
	void OnIncomingMessage(const Tizen::Base::String& message);
	void SavePrivateChat(const Tizen::Base::String& editArea, const Tizen::Base::String& editField);
	void SavePrivateChatEditField(const Tizen::Base::String& editField);
	void SavePrivateChatEditArea(const Tizen::Base::String& editArea);
	Tizen::Base::String GetDeviceAddress(void) const;
	Tizen::Base::String GetPrivateChatArea(void) const;
	Tizen::Base::String GetPrivateChatField(void) const;

	bool operator ==(const ChatTab& rhs) const;
	bool operator !=(const ChatTab& rhs) const;

private:
	Tizen::Base::String __privateChatEditField;
	Tizen::Base::String __privateChatEditArea;
	Tizen::Base::String __deviceAddress;

	int __tabActionId;
};

class ChatForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::ITextEventListener
	, public Tizen::Ui::ITouchEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
	, public Tizen::Net::INetConnectionEventListener
	, public Tizen::Net::Sockets::ISocketEventListener
{
public:
	ChatForm(void);
	virtual ~ChatForm(void);

	bool Initialize(void);

	void InitiatePrivateChat(const Tizen::Base::String& deviceName, const Tizen::Base::String& ipAddress);
	void FinishPrivateChat(void);

	void SwitchPrivateChatTab(int oldIndex, int tabIndex);
	void SwitchToBroadcastTab(void);

	void SendPrivateChatMessage(void);
	void SendBroadcastChatMessage(void);

	void DisplayGroupMembers(const Tizen::Base::Collection::IList* pWifiDirectDeviceInfoList);

	void ShowMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text);

	// Form
	virtual result OnInitializing(void);

	// IActionEventListener
	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);

	// ITextEventListener
	virtual void OnTextValueChangeCanceled(const Tizen::Ui::Control& source);
	virtual void OnTextValueChanged(const Tizen::Ui::Control& source);

	// ITouchEventListener
	virtual void OnTouchDoublePressed(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition,
										const Tizen::Ui::TouchEventInfo& touchInfo){}
	virtual void OnTouchFocusIn(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition,
										const Tizen::Ui::TouchEventInfo& touchInfo){}
	virtual void OnTouchFocusOut(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition,
										const Tizen::Ui::TouchEventInfo& touchInfo){}
	virtual void OnTouchLongPressed(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition,
										const Tizen::Ui::TouchEventInfo& touchInfo){}
	virtual void OnTouchMoved(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition,
										const Tizen::Ui::TouchEventInfo& touchInfo){}
	virtual void OnTouchPressed(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition,
										const Tizen::Ui::TouchEventInfo& touchInfo){}
	virtual void OnTouchReleased(const Tizen::Ui::Control& source, const Tizen::Graphics::Point& currentPosition,
										const Tizen::Ui::TouchEventInfo& touchInfo);

	// IFormBackEventListener
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	// ISceneEventListener
	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
	                                const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
	                                const Tizen::Ui::Scenes::SceneId &nextSceneId){}

	// INetConnectionEventListener
	virtual void OnNetConnectionStarted(Tizen::Net::NetConnection& netConnection, result r);
	virtual void OnNetConnectionStopped(Tizen::Net::NetConnection& netConnection, result r);
	virtual void OnNetConnectionSuspended(Tizen::Net::NetConnection& netConnection) {};
	virtual void OnNetConnectionResumed(Tizen::Net::NetConnection& netConnection) {};

	// ISocketEventListener
	virtual void OnSocketConnected(Tizen::Net::Sockets::Socket& socket) {};
	virtual void OnSocketClosed(Tizen::Net::Sockets::Socket& socket, Tizen::Net::Sockets::NetSocketClosedReason reason) {}
	virtual void OnSocketReadyToReceive(Tizen::Net::Sockets::Socket& socket);
	virtual void OnSocketReadyToSend(Tizen::Net::Sockets::Socket& socket) {}
	virtual void OnSocketAccept(Tizen::Net::Sockets::Socket& socket) {}

private:
	void RequestGroupMemberInfo(void);
	result InitializeChatSocket(void);
	int GetActionId(void);
	void FreeActionId(int actionId);
	bool IsFreeActionId(int actionId);
	int GetItemActionIdAt(int tabIndex);
	int GetItemIndexFromActionId(int actionId);
	void RemoveItemActionId(int tabIndex);
	void UpdateFooterControl(void);

private:
	static const int MAX_TAB_COUNT = 8;

	static const int ID_OPTION_SEND = 100;
	static const int ID_OPTION_CLEAR_CHAT = 101;
	static const int ID_OPTION_CHAT_MEMBERS = 102;
	static const int ID_OPTION_END_CHAT = 103;
	static const int ID_TAB_BROADCAST = 104;
	static const int MESSAGE_BOX_TIMEOUT = 2000;

	static const int MAX_TAB = 10;
	static const int EDIT_FIELD_HEIGHT = 112;
	static const int EDIT_AREA_HEIGHT = 350;
	static const int POPUP_TEXT_SIZE = 40;

	static const int MAX_NO_CONTROL = 3;

	Tizen::Base::String __localIpAddress;
	Tizen::Base::String __localDeviceName;
	Tizen::Base::String __broadcastMessageHeader;
	Tizen::Ui::Controls::TabBar* __pTabBarChat;
	Tizen::Ui::Controls::EditArea* __pEditAreaBroadcastChat;
	Tizen::Ui::Controls::EditArea* __pEditAreaPrivateChat;
	Tizen::Ui::Controls::EditField* __pEditFieldBroadcastChat;
	Tizen::Ui::Controls::EditField* __pEditFieldPrivateChat;
	Tizen::Ui::Controls::Popup* __pPopupRequestingInfo;
	Tizen::Ui::Controls::Popup* __pPopupInitializingChat;
	Tizen::Ui::Controls::Label* __pLabelInitializingChat;

	Tizen::Base::Collection::ArrayListT<ChatTab> __privateChatList;

	Tizen::Net::Wifi::WifiDirectGroupMemberType __wifiDirectGroupMemberType;
	Tizen::Net::Wifi::WifiDirectDevice* __pWifiDirectDevice;
	Tizen::Net::NetConnection* __pNetConnection;
	Tizen::Net::Sockets::Socket* __pUdpSocket;

	int __actionIdArray[MAX_TAB_COUNT];
	int __tabIndexArray[MAX_TAB_COUNT];
	int __lastTabIndex;
	int __lastTabActionId;
	int __keypadActionId;
	int __chatPortNumber;
	bool __isKeypadEnabled;
};

#endif // _CHAT_FORM_H_
