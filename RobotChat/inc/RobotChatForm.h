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
#include <FMedia.h>

using namespace Tizen::Base;
using namespace Tizen::Graphics;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Media;

class RobotChatForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
	, public Tizen::Net::INetConnectionEventListener
	, public Tizen::Net::Sockets::ISocketEventListener
	, public Tizen::Media::IPlayerEventListener
	, public Tizen::Media::IPlayerVideoEventListener
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

	// IPlayerEventListener
	virtual void OnPlayerOpened(result r){}
	virtual void OnPlayerEndOfClip(void){}
	virtual void OnPlayerBuffering(int percent){}
	virtual void OnPlayerErrorOccurred(Tizen::Media::PlayerErrorReason r){}
	virtual void OnPlayerInterrupted(void){}
	virtual void OnPlayerReleased(void){}
	virtual void OnPlayerSeekCompleted(result r){}
	virtual void OnPlayerAudioFocusChanged(void){}

	// IPlayerVideoEventListener
	virtual void OnVideoFrameDecoded(Player &src, BitmapPixelFormat bitmapPixelFormat, const Dimension &dim,
	                                      const byte *pBuffer, int sizeOfBuffer, result r);

private:
	Tizen::Base::String __localIpAddress;
	Tizen::Base::String __localDeviceName;
	Tizen::Base::String __broadcastMessageHeader;

	Tizen::Net::Wifi::WifiDirectDevice* __pWifiDirectDevice;
	Tizen::Net::NetConnection* __pNetConnection;
	Tizen::Net::Sockets::Socket* __pUdpSocket;

	int __chatPortNumber;

	std::unique_ptr<Tizen::Media::Player> __pPlayer;
	std::unique_ptr<Tizen::Ui::Controls::OverlayRegion> __pOverlay;

	result InitializeChatSocket(void);
	result StartVideoWithVideoEventListener(String filePath);
};


#endif /* ROBOTCHATFORM_H_ */
