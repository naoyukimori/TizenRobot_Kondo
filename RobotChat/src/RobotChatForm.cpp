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
using namespace Tizen::Media;

RobotChatForm::RobotChatForm()
	: __localIpAddress()
	, __localDeviceName()
	, __broadcastMessageHeader(L"WifiDirectBroadcast")
	, __pWifiDirectDevice(null)
	, __pNetConnection(null)
	, __pUdpSocket(null)
	, __pPlayer(null)
	, __pOverlay(null)
{

}

RobotChatForm::~RobotChatForm()
{
	delete __pUdpSocket;
	delete __pNetConnection;

	__pPlayer->Close();
}

bool
RobotChatForm::Initialize(void)
{
	VerticalBoxLayout formLayout;
	formLayout.Construct(VERTICAL_DIRECTION_DOWNWARD);

	Construct(formLayout, FORM_STYLE_NORMAL);
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

	__pPlayer.reset(new (std::nothrow) Player());
	if ( __pPlayer.get() == null)
	{
		AppLogException("pPlayer = new (std::nothrow) Player() has failed");
		return E_FAILURE;
	}

	StartVideoWithVideoEventListener(L"");

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

/*
void
RobotChatForm::CreateOverlayRegion(void)
{
	bool modified = false;
	bool isValidRect = false;
	Rectangle rect = GetClientAreaBounds();

	__pPanel.reset(Tizen::Ui::Controls::Form::GetOverlayRegionN(rect, OVERLAY_REGION_TYPE_NORMAL));
	TryReturn(__pPanel.get() != null, ,"GetOverlayRegion Failed");
	AppLog("__pPanel : %s", GetErrorMessage(GetLastResult()));
	__pPanel->Show();
}
*/

result
RobotChatForm::StartVideoWithVideoEventListener(String path)
{
	result r = E_SUCCESS;
    Rectangle rect = GetClientAreaBounds();
    String filePath = Tizen::App::App::GetInstance()->GetAppRootPath() + L"res/Video/sampleH264.mp4";

    r = __pPlayer->Construct(*this, *this);
    if (IsFailed(r))
    {
        return r;
    }

    r = __pPlayer->OpenFile(filePath);
    if (IsFailed(r))
    {
    	return r;
    }

    // Gets OverlayRegion from this Form
    __pOverlay.reset(Tizen::Ui::Controls::Form::GetOverlayRegionN(rect, OVERLAY_REGION_TYPE_NORMAL));
    if (__pOverlay == null)
    {
    	return GetLastResult();
    }

    r = __pPlayer->Play();
    if (IsFailed(r))
    {
    	return r;
    }

    return E_SUCCESS;
}

void
RobotChatForm::OnVideoFrameDecoded(Player &src, BitmapPixelFormat bitmapPixelFormat, const Dimension &dim,
										const byte *pBuffer, int sizeOfBuffer, result r)
{
    ByteBuffer buf;
    OverlayRegionBufferPixelFormat overlayPixelFormat;

    if (IsFailed(r))
    {
        return;
    }

    if (__pOverlay == null)
    {
        return;
    }

    if (bitmapPixelFormat == BITMAP_PIXEL_FORMAT_ARGB8888)
    {
        overlayPixelFormat = OVERLAY_REGION_BUFFER_PIXEL_FORMAT_ARGB8888;
    }
    else if (bitmapPixelFormat == BITMAP_PIXEL_FORMAT_RGB565)
    {
        overlayPixelFormat = OVERLAY_REGION_BUFFER_PIXEL_FORMAT_RGB565;
    }
    else // Unsupported pixel format
    {
        return;
    }

    buf.Construct(pBuffer, 0, sizeOfBuffer, sizeOfBuffer);
    __pOverlay->SetInputBuffer(buf, dim, overlayPixelFormat);
}
