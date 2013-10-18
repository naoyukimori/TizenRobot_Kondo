/*
 * RobotChatForm.h
 *
 *  Created on: Oct 18, 2013
 *      Author: kien
 */

#ifndef ROBOTCHATFORM_H_
#define ROBOTCHATFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FNet.h>

class RobotChatForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
	, public Tizen::Net::INetConnectionEventListener
	, public Tizen::Net::Sockets::ISocketEventListener
{
public:
	RobotChatForm(void);
	virtual ~RobotChatForm(void);

	bool Initialize(void);

	void ShowMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text);

	// Form
	virtual result OnInitializing(void);

	// IFormBackEventListener
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	// ISceneEventListener
	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
	                                const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
	                                const Tizen::Ui::Scenes::SceneId &nextSceneId){}

	// INetConnectionEventListener
	virtual void OnNetConnectionStarted(Tizen::Net::NetConnection& netConnection, result r) {}
	virtual void OnNetConnectionStopped(Tizen::Net::NetConnection& netConnection, result r) {}
	virtual void OnNetConnectionSuspended(Tizen::Net::NetConnection& netConnection) {};
	virtual void OnNetConnectionResumed(Tizen::Net::NetConnection& netConnection) {};

	// ISocketEventListener
	virtual void OnSocketConnected(Tizen::Net::Sockets::Socket& socket) {};
	virtual void OnSocketClosed(Tizen::Net::Sockets::Socket& socket, Tizen::Net::Sockets::NetSocketClosedReason reason) {}
	virtual void OnSocketReadyToReceive(Tizen::Net::Sockets::Socket& socket);
	virtual void OnSocketReadyToSend(Tizen::Net::Sockets::Socket& socket) {}
	virtual void OnSocketAccept(Tizen::Net::Sockets::Socket& socket) {}

private:
	Tizen::Base::String __localIpAddress;
	Tizen::Base::String __localDeviceName;
	Tizen::Base::String __broadcastMessageHeader;

	Tizen::Net::Wifi::WifiDirectDevice* __pWifiDirectDevice;
	Tizen::Net::NetConnection* __pNetConnection;
	Tizen::Net::Sockets::Socket* __pUdpSocket;

	int __chatPortNumber;
};


#endif /* ROBOTCHATFORM_H_ */
