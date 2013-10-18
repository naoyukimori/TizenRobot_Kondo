/*
 * RobotChatForm.cpp
 *
 *  Created on: Oct 18, 2013
 *      Author: kien
 */

#include "RobotChatForm.h"
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

RobotChatForm::RobotChatForm()
	: __localIpAddress()
	, __localDeviceName()
	, __broadcastMessageHeader(L"WifiDirectBroadcast")
	, __pWifiDirectDevice(null)
	, __pNetConnection(null)
	, __pUdpSocket(null)
{

}

RobotChatForm::~RobotChatForm()
{
	delete __pUdpSocket;
	delete __pNetConnection;
}

bool
RobotChatForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL);
	SetName(FORM_ROBOT_CHAT);

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

result
RobotChatForm::OnInitializing(void)
{
	SetOrientation(ORIENTATION_LANDSCAPE);
	SetFormBackEventListener(this);

	Draw(true);
	return E_SUCCESS;
}

void
RobotChatForm::OnFormBackRequested(Form& source)
{
	if (__pNetConnection != null)
	{
		if (__pNetConnection->Stop() == E_SUCCESS)
		{
			// Do something here.
		}
		else
		{
			AppLog("Failed to Stop NetConnection");
		}
		SceneManager* pSceneManager = SceneManager::GetInstance();
		AppAssert(pSceneManager);

		pSceneManager->GoBackward(BackwardSceneTransition());
	}
}

void
RobotChatForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    TryReturnVoid(pArgs != null, "The argument list is null");

    __pWifiDirectDevice = static_cast<WifiDirectDevice*>(pArgs->GetAt(0));

    String* pPortNumber = static_cast<String*>(pArgs->GetAt(1));
    Integer::Decode(*pPortNumber, __chatPortNumber);

    delete pArgs;

    WifiDirectDeviceInfo* pWifiDirectDeviceInfo = __pWifiDirectDevice->GetLocalDeviceInfoN();
    TryReturnVoid(pWifiDirectDeviceInfo != null, "Failed to get Device Info");

    __localDeviceName.Append(pWifiDirectDeviceInfo->GetDeviceName());

    delete pWifiDirectDeviceInfo;

    RequestRedraw();
}

void
RobotChatForm::OnSocketReadyToReceive(Socket& socket)
{
	result r = E_SUCCESS;
	String message;
	unsigned long arg = 0;
	Ip4Address ip4Address("0"); //ADDR_ANY
	String deviceName(L"");
	int deviceIndex = 0;

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
		message.Remove(0, __broadcastMessageHeader.GetLength());
	}
	else
	{

	}
}
