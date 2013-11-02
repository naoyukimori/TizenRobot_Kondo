/*
 * RobotChatForm.cpp
 *
 *  Created on: Oct 18, 2013
 *      Author: kien
 */
#include <FSystem.h>
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
using namespace Tizen::Io;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base::Utility;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Web::Json;
using namespace Tizen::Media;
using namespace Tizen::System;

RobotChatForm::RobotChatForm()
	: __localIpAddress()
	, __localDeviceName()
	, __broadcastMessageHeader(L"WifiDirectBroadcast")
	, __pWifiDirectDevice(null)
	, __pNetConnection(null)
	, __pUdpSocket(null)
	, __chatPortNumber(0)
	, __pMutex(null)
	, __pMediaPath(null)
{
	__pResourcePath = Environment::GetExternalStoragePath() + L"Robot/";
}

RobotChatForm::~RobotChatForm()
{
	if (__pUdpSocket)
	{
		__pUdpSocket->Close();
		__pUdpSocket->RemoveSocketListener(*(ISocketEventListener*) this);
		delete __pUdpSocket;
		__pUdpSocket = null;
	}
	if (__pNetConnection)
	{
		if (__pNetConnection->Stop() == E_SUCCESS)
		{
			// Do something here.
		}
		else
		{
			AppLog("Failed to Stop NetConnection");
		}
		__pNetConnection->RemoveNetConnectionListener(*(INetConnectionEventListener*) this);
		delete __pNetConnection;
		__pNetConnection = null;
	}

	if (__fileList.GetCount() > 0)
	{
		__fileList.RemoveAll(true);
	}

	if (__pMutex)
	{
		 delete __pMutex;
	}

	if (_pPatterns)
	{
		_pPatterns->RemoveAll();
		delete _pPatterns;
		_pPatterns = null;
	}
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

    /* initialize & activate Bluetooth and connect to KBT-1 */
	if (kbt.Initialize())
	{
		kbt.Connect();
	}

	__pTimer.Construct(*this);

	return true;
}

void
RobotChatForm::CreateGallery(void)
{
	__pGallery = new Gallery();
	__pGallery->Gallery::Construct(GetBoundsF());
	__pGallery->SetItemProvider(*this);
	AddControl(__pGallery);
}


void
RobotChatForm::GeneratePattern(void)
{
	File file;
	String filePath = __pResourcePath + L"Data/patterns.json";

	//Read File Content into buffer
	result r = file.Construct(filePath, L"r");
	TryReturnVoid(r == E_SUCCESS, "file construction failure with [%s]", GetErrorMessage(r));

	FileAttributes att;
	r = File::GetAttributes(filePath, att);
	TryReturnVoid(r == E_SUCCESS, "file GetAttributes failure with [%s]", GetErrorMessage(r));

	long long size = att.GetFileSize();
	TryReturnVoid(size > 0, "file does not contain valid data, size of file [%ld]", size);

	ByteBuffer buf;
	r = buf.Construct(size + 1);
	TryReturnVoid(r == E_SUCCESS, "bytebuffer construct failure with [%s]", GetErrorMessage(r));

	r = file.Read(buf);
	TryReturnVoid(r == E_SUCCESS, "file read failure with [%s]", GetErrorMessage(r));

	_pPatterns->RemoveAll(true);
	//Call Json Parser
	IJsonValue* pJson = JsonParser::ParseN(buf);
	JsonObject* pObject = static_cast< JsonObject* >(pJson);
	IMapEnumeratorT< const String*, IJsonValue* >* pMapEnum = pObject->GetMapEnumeratorN();

	IJsonValue* currentValue = null;
	while (pMapEnum->MoveNext() == E_SUCCESS)
	{
		const String* key = null;
		pMapEnum->GetKey(key);
		pMapEnum->GetValue(currentValue);
		if (currentValue->GetType() == JSON_TYPE_ARRAY && key->Equals(L"Robot patterns", true))
		{
			JsonArray* pJsonArray = static_cast< JsonArray* >(currentValue);
			pJsonArray->GetCount();
			IEnumeratorT< IJsonValue* >* pEnum = pJsonArray->GetEnumeratorN();
			while (pEnum->MoveNext() == E_SUCCESS)
			{
				Pattern* pPattern = new (std::nothrow) Pattern();
				pEnum->GetCurrent(currentValue);
				if (currentValue->GetType() == JSON_TYPE_OBJECT)
				{
					JsonObject* pObject = static_cast< JsonObject* >(currentValue);
					IMapEnumeratorT< const String*, IJsonValue* >* pMapEnum = pObject->GetMapEnumeratorN();
					while (pMapEnum->MoveNext() == E_SUCCESS)
					{
						const String* key;
						pMapEnum->GetKey(key);
						pMapEnum->GetValue(currentValue);
						if (key->Equals(L"name", true))
						{
							JsonString* pVal = static_cast< JsonString* >(currentValue);
							pPattern->SetName(pVal);
						}
						else if (key->Equals(L"type", true))
						{
							JsonString* pVal = static_cast< JsonString* >(currentValue);
							pPattern->SetType(pVal);
						}
						else if (key->Equals(L"description", true))
						{
							JsonString* pVal = static_cast< JsonString* >(currentValue);
							pPattern->SetDescription(pVal);
						}
						else if (key->Equals(L"filename", true))
						{
							JsonString* pVal = static_cast< JsonString* >(currentValue);
							pPattern->SetMediaFileName(pVal);
						}
						else if (key->Equals(L"duration", true))
						{
							JsonNumber* pVal = static_cast< JsonNumber* >(currentValue);
							pPattern->SetDuration((unsigned int)(pVal->ToInt()));
						}
						else if (key->Equals(L"motion", true))
						{
							JsonNumber* pVal = static_cast< JsonNumber* >(currentValue);
							pPattern->SetMotionNumber((unsigned int)(pVal->ToInt()));
						}
						else if (key->Equals(L"delay", true))
						{
							JsonNumber* pVal = static_cast< JsonNumber* >(currentValue);
							pPattern->SetDelay((unsigned int)(pVal->ToInt()));
						}
					}
					delete pMapEnum;
				}
				_pPatterns->Add((Object*)pPattern);
			}
			delete pEnum;
			break;
		}
	}
	delete pMapEnum;

	return;

CATCH:
	return;
}

result
RobotChatForm::OnInitializing(void)
{
	result r = E_SUCCESS;
	SetOrientation(ORIENTATION_LANDSCAPE);

	SetFormBackEventListener(this);

	__pMutex = new (std::nothrow) Mutex();
	r = __pMutex->Create();

	_pPatterns = new (std::nothrow) ArrayList();
	r = _pPatterns->Construct();
	GeneratePattern();

	GetFilesList();
	CreateGallery();

	return E_SUCCESS;
}

void
RobotChatForm::OnFormBackRequested(Form& source)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition());
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

result
RobotChatForm::InitializeChatSocket(void)
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


void
RobotChatForm::OnNetConnectionStarted(NetConnection& netConnection, result r)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	if (r == E_SUCCESS)
	{
		if (netConnection.GetNetConnectionInfo() && netConnection.GetNetConnectionInfo()->GetLocalAddress())
		{
			__localIpAddress = netConnection.GetNetConnectionInfo()->GetLocalAddress()->ToString();
		}

		r = InitializeChatSocket();
		if (IsFailed(r))
		{
			AppLog("Failed to create socket. [%s]", GetErrorMessage(r));
			pSceneManager->GoBackward(BackwardSceneTransition());
		}
	}
	else
	{
		AppLog("Net Connection Failed. [%s]", GetErrorMessage(r));
		pSceneManager->GoBackward(BackwardSceneTransition());
	}

}

void
RobotChatForm::OnNetConnectionStopped(NetConnection& netConnection, result r)
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

	Pattern* pPattern = null;
	if (__pMediaPath)
	{
		delete __pMediaPath;
		__pMediaPath = null;
	}

	unsigned int pMotionIndex = 0;
	for (int i = 0; i < _pPatterns->GetCount(); i++)
	{
		pPattern = (Pattern*)(_pPatterns->GetAt(i));
		if (pPattern)
		{
			String* pName = pPattern->GetName();
			if (pName)
			{
				const String* str = new String(pName->GetPointer());
				if (message.Contains(*str))
				{
					__pMediaPath = new String(__pResourcePath.GetPointer());
					__pMediaPath->Append(pPattern->GetMediaFilePath().GetPointer());
					pMotionIndex = pPattern->GetMotionNumber();
					break;
				}
			}
		}
	}

	if (__pMediaPath && pMotionIndex > 0)
	{
		kbt.PlayMotion(pMotionIndex);
		__pTimer.Start(100);
	}
}

int
RobotChatForm::GetItemCount(void)
{
	return __fileList.GetCount();
}

GalleryItem*
RobotChatForm::CreateItem(int index)
{
	GalleryItem* pGallery = new (std::nothrow) GalleryItem();
	String* fileName = static_cast<String*>(__fileList.GetAt(index));
	if (fileName)
	{
		Image* pImage = new (std::nothrow) Image();
		pImage->Construct();
		Bitmap* __pImageTemp;

		__pMutex->Acquire();
		__pImageTemp = pImage->DecodeN(*fileName, BITMAP_PIXEL_FORMAT_ARGB8888);
		pGallery->Construct(*__pImageTemp, *fileName);

		delete pImage;
		__pMutex->Release();
	}

	return pGallery;
}

bool
RobotChatForm::DeleteItem(int index, GalleryItem* pItem)
{
	delete pItem;
	return true;
}


result
RobotChatForm::IsImageFile(String* pFileName)
{
	ImageFormat format = IMG_FORMAT_NONE;
	int width = 0;
	int height = 0;
	result r = E_SUCCESS;

	r = ImageBuffer::GetImageInfo(*pFileName, format, width, height);
	if (r != E_SUCCESS)
	{
		AppLogException("Could not get image information %S", pFileName->GetPointer());
		return r;

	}
	if (format != IMG_FORMAT_NONE)
	{
		return E_SUCCESS;
	}
	return E_UNSUPPORTED_FORMAT;
}

result
RobotChatForm::GetFilesList()
{
	Directory* pDir = null;
	DirEnumerator* pDirEnum = null;
	StringComparer strComparer;
	result r = E_SUCCESS;

	__fileList.Construct();

	String pDirPath(__pResourcePath + L"Image");
	pDir = new (std::nothrow) Directory();
	pDir->Construct(pDirPath);

	TryCatch(r == E_SUCCESS, delete pDir ,"[%s] Failed to construct image directory", GetErrorMessage(r));

	pDirEnum = pDir->ReadN();

	TryCatch(pDirEnum != null, delete pDir ,"[%s] Failed to read entries from image directory", GetErrorMessage(GetLastResult()));

	while (pDirEnum->MoveNext() == E_SUCCESS)
	{
		DirEntry dirEntry = pDirEnum->GetCurrentDirEntry();
		if ((dirEntry.IsDirectory() == false) && (dirEntry.IsHidden() == false))
		{
			String* fullFileName = new (std::nothrow) String;
			fullFileName->Append(pDirPath);
			fullFileName->Append('/');
			String fileName(dirEntry.GetName());
			fullFileName->Append(fileName);

			r = IsImageFile(fullFileName);
			if (r == E_SUCCESS)
			{
				__fileList.Add(fullFileName);
			}
			else
			{
				AppLog("%S is not image file.", fullFileName->GetPointer());
				delete fullFileName;
				fullFileName = null;
			}
		}
	}

	__fileList.Sort(strComparer);

	delete pDir;
	delete pDirEnum;

	if (__fileList.GetCount() >= 0)
	{
		return E_SUCCESS;
	}
	else
	{
		return E_FAILURE;
	}
CATCH:
	return r;
}

void
RobotChatForm::OnTimerExpired(Timer &timer)
{
	ArrayList* pArgs = new ArrayList();
	pArgs->Add(__pMediaPath);
	SceneManager::GetInstance()->GoForward(ForwardSceneTransition(SCENE_VIDEO_FORM), pArgs);
}
