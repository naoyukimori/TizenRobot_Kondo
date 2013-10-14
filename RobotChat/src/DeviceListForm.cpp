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

#include "DeviceListForm.h"
#include "MainForm.h"
#include "ChatForm.h"
#include "FormFactory.h"

using namespace std;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Graphics;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Media;
using namespace Tizen::Base::Collection;
using namespace Tizen::Net::Wifi;
using namespace Tizen::Ui::Scenes;

DeviceListForm::DeviceListForm(RequestId requestId)
	: __pGroupedListView(null)
	, __pBitmapTelephone(null)
	, __pBitmapComputer(null)
	, __pBitmapInputDevice(null)
	, __pBitmapDefault(null)
	, __pDeviceInfoList(null)
	, __selectedGroupIndex(-1)
	, __selectedItemIndex(-1)
	, __groupOwnerCount(0)
	, __groupClientCount(0)
	, __isGroupOwner(false)
	, __requestId(requestId)
{
}

DeviceListForm::~DeviceListForm(void)
{
}

bool
DeviceListForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL | FORM_STYLE_HEADER | FORM_STYLE_INDICATOR | FORM_STYLE_FOOTER);

	SetName(FORM_DEVICE_LIST);

	return true;
}

result
DeviceListForm::OnInitializing(void)
{
	Header* pHeader = GetHeader();
	if (pHeader != null)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);

		if(__requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
	    {
		    pHeader->SetTitleText(L"Select peer");
	    }
		else if (__requestId == FormFactory::REQUEST_CHAT_MEMBER_DEVICE_LIST_FORM)
        {
            pHeader->SetTitleText(L"Chat member list");
        }
		else if (__requestId == FormFactory::REQUEST_PEER_DEVICE_LIST_FORM)
        {
            pHeader->SetTitleText(L"Peer list");
        }

	}

	// Setup back event listener
	SetFormBackEventListener(this);

	Image image;
	image.Construct();

	__pBitmapTelephone.reset(image.DecodeN(App::GetInstance()->GetAppRootPath() + L"data/Telephone.png", BITMAP_PIXEL_FORMAT_RGB565));
	TryReturn(__pBitmapTelephone != null, GetLastResult(), "Getting Telephone image failed.");

	__pBitmapComputer.reset(image.DecodeN(App::GetInstance()->GetAppRootPath() + L"data/Computer.png", BITMAP_PIXEL_FORMAT_RGB565));
	TryReturn(__pBitmapComputer != null, GetLastResult(), "Getting Computer image failed.");

	__pBitmapInputDevice.reset(image.DecodeN(App::GetInstance()->GetAppRootPath() + "data/InputDevice.png", BITMAP_PIXEL_FORMAT_RGB565));
	TryReturn(__pBitmapInputDevice != null, GetLastResult(), "Getting InputDevice image failed.");

	__pBitmapDefault.reset(image.DecodeN(App::GetInstance()->GetAppRootPath() + L"data/Default.png", BITMAP_PIXEL_FORMAT_RGB565));
	TryReturn(__pBitmapDefault != null, GetLastResult(), "Getting Default image failed.");

	Draw(true);

	return E_SUCCESS;
}

void
DeviceListForm::ShowMessageBox(const String& title, const String& text)
{
	MessageBox messageBox;
	int modalResult = 0;

	static const int MESSAGE_BOX_TIMEOUT = 2000;

	messageBox.Construct(title, text, MSGBOX_STYLE_NONE, MESSAGE_BOX_TIMEOUT);
	messageBox.ShowAndWait(modalResult);
}

void
DeviceListForm::OnActionPerformed(const Tizen::Ui::Control& source, int actionId)
{
    MainForm* pMainForm = static_cast< MainForm* >(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl(FORM_MAIN));
	ChatForm* pChatForm = static_cast< ChatForm* >(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl(FORM_CHAT));

	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	int deviceIndex = (__selectedGroupIndex * __groupOwnerCount) + __selectedItemIndex;

	switch (actionId)
	{
	// REQUEST_SCAN_RESULT_DEVICE_LIST_FORM
	case ID_OPTION_SCAN:
		__pGroupedListView->UpdateList();
		pMainForm->Scan();
		break;
	// REQUEST_CHAT_MEMBER_DEVICE_LIST_FORM
	case ID_OPTION_CHAT:
		if (deviceIndex < 0)
		{
			ShowMessageBox(L"Chat Member List", L"Please Select a Device to Chat");
		}
		else
		{
			WifiDirectDeviceInfo* pPeerDeviceInfo = static_cast<WifiDirectDeviceInfo*>(__pDeviceInfoList->GetAt(deviceIndex));
			if (!pPeerDeviceInfo->GetIpAddress())
			{
				ShowMessageBox(L"Chat Member List", L"Cannot Start Chat As IP Address Is Not Available");
				return;
			}

			pChatForm->InitiatePrivateChat(pPeerDeviceInfo->GetDeviceName(), pPeerDeviceInfo->GetIpAddress()->ToString());
			pSceneManager->GoBackward(BackwardSceneTransition());
		}
		break;
    // REQUEST_PEER_DEVICE_LIST_FORM
    case ID_OPTION_DISCONNECT:
    {
		if (deviceIndex < 0)
		{
			ShowMessageBox(L"Peer List", L"Please Select a Device to Disconnect");
		}
		else
		{
			WifiDirectDeviceInfo* pDeviceInfo = new WifiDirectDeviceInfo(*static_cast<WifiDirectDeviceInfo*>(__pDeviceInfoList->GetAt(deviceIndex)));

			ArrayList* pArgs = new ArrayList(SingleObjectDeleter);
			pArgs->Add(*pDeviceInfo);

			SceneManager* pSceneManager = SceneManager::GetInstance();
			AppAssert(pSceneManager);
			pSceneManager->GoBackward(BackwardSceneTransition(), pArgs);
		}
    }
        break;
	default:
		break;
	}
}

void
DeviceListForm::OnGroupedListViewItemStateChanged(GroupedListView& listView, int groupIndex, int itemIndex, int elementId, ListItemStatus state)
{
    if (state == LIST_ITEM_STATUS_SELECTED && __requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
    	WifiDirectDeviceInfo* pDeviceInfo = null;

    	if (groupIndex == INDEX_GROUP_OWNER)
    	{
    		pDeviceInfo = new WifiDirectDeviceInfo(*static_cast<WifiDirectDeviceInfo*>(__groupOwnerInfoList.GetAt(itemIndex)));
    	}
    	else
		{
    		pDeviceInfo = new WifiDirectDeviceInfo(*static_cast<WifiDirectDeviceInfo*>(__othersInfoList.GetAt(itemIndex)));
		}

        ArrayList* pArgs = new ArrayList(SingleObjectDeleter);
        pArgs->Add(*pDeviceInfo);

        SceneManager* pSceneManager = SceneManager::GetInstance();
        AppAssert(pSceneManager);
        pSceneManager->GoBackward(BackwardSceneTransition(), pArgs);
    }

	if (state == LIST_ITEM_STATUS_CHECKED)
	{
		// uncheck the previously selected item
		__pGroupedListView->SetItemChecked(__selectedGroupIndex, __selectedItemIndex, false);
		__pGroupedListView->RequestRedraw();
		__selectedGroupIndex = groupIndex;
		__selectedItemIndex = itemIndex;
	}
	else if (state == LIST_ITEM_STATUS_UNCHECKED)
	{
		__selectedGroupIndex = -1;
		__selectedItemIndex = -1;
	}
}

int
DeviceListForm::GetGroupCount(void)
{
    static const int DEVICE_TYPE_COUNT = 2;
    static const int DEVICE_INFO_GROUP_COUNT = 1;

    if (__requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
        return DEVICE_TYPE_COUNT;
    }
    else
    {
        return DEVICE_INFO_GROUP_COUNT;
    }
}

int
DeviceListForm::GetItemCount(int groupIndex)
{
	int itemCount = 0;

	TryReturn(__pDeviceInfoList != null, 0, "Scan result is not exist");

	if (__requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
	    if (groupIndex == INDEX_GROUP_OWNER)
	    {
	        itemCount = __groupOwnerInfoList.GetCount();
	    }
	    else
	    {
	        itemCount = __othersInfoList.GetCount();
	    }
	}
	else
    {
	    if (__isGroupOwner == true)
	    {
	        itemCount = __groupClientCount;
	    }
	    else
	    {
	        itemCount = __groupOwnerCount;
	    }

    }

	return itemCount;
}

GroupItem*
DeviceListForm::CreateGroupItem(int groupIndex, int itemWidth)
{
    static const int GROUP_ITEM_HEIGHT = 50;

	GroupItem* pItem = new GroupItem();
	pItem->Construct(Dimension(GetClientAreaBounds().width, GROUP_ITEM_HEIGHT));

	String text;

    if (__requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
        if (groupIndex == 0)
        {
            text = L"Group Owner";
        }
        else if(groupIndex ==1)
        {
            text = L"Other Device";
        }
    }
    else
    {
        if (__isGroupOwner == true)
       {
            text = L"Group Client";
       }
       else
       {
           text = L"Group Owner";
       }
    }

	pItem->SetElement(text, null);
	return pItem;
}

ListItemBase*
DeviceListForm::CreateItem(int groupIndex, int itemIndex, int itemWidth)
{
    CustomItem* pItem = new CustomItem();

    static const int LIST_ITEM_HEIGHT = 100;

    if (__requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
        pItem->Construct(Tizen::Graphics::Dimension(GetClientAreaBounds().width, LIST_ITEM_HEIGHT), LIST_ANNEX_STYLE_NORMAL);
    }
    else
    {
        pItem->Construct(Tizen::Graphics::Dimension(GetClientAreaBounds().width, LIST_ITEM_HEIGHT), LIST_ANNEX_STYLE_MARK);
    }


    int listIndex = itemIndex;
    WifiDirectDeviceInfo* pDeviceInfo = null;

    if (__requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
        if (groupIndex == INDEX_GROUP_OWNER)
        {
            pDeviceInfo = static_cast<WifiDirectDeviceInfo*>(__groupOwnerInfoList.GetAt(itemIndex));
        }
        else
        {
            pDeviceInfo = static_cast<WifiDirectDeviceInfo*>(__othersInfoList.GetAt(itemIndex));
        }
    }
    else
    {
        pDeviceInfo = static_cast<WifiDirectDeviceInfo*>(__pDeviceInfoList->GetAt(listIndex));
    }



    String deviceName = pDeviceInfo->GetDeviceName();
    String displayAddress;

    if (__requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
        displayAddress.Append(pDeviceInfo->GetMacAddress());
    }
    else if (pDeviceInfo->GetIpAddress() != null)
    {
        displayAddress.Append(pDeviceInfo->GetIpAddress()->ToString());
    }


    static const int BITMAP_ITEM_X = 40;
    static const int BITMAP_ITEM_Y = 40;
    static const int BITMAP_ITEM_WIDTH = 20;
    static const int BITMAP_ITEM_HEIGHT = 20;

    static const int ID_LIST_BITMAP = 103;



    switch (pDeviceInfo->GetDeviceTypeCategory())
    {
    case WIFI_DIRECT_DEVICE_TYPE_CATEGORY_TELEPHONE:
        pItem->AddElement(Rectangle(BITMAP_ITEM_X, BITMAP_ITEM_Y, BITMAP_ITEM_WIDTH, BITMAP_ITEM_HEIGHT), ID_LIST_BITMAP, *__pBitmapTelephone, null, null);
        break;

    case WIFI_DIRECT_DEVICE_TYPE_CATEGORY_COMPUTER:
        pItem->AddElement(Rectangle(BITMAP_ITEM_X, BITMAP_ITEM_Y, BITMAP_ITEM_WIDTH, BITMAP_ITEM_HEIGHT), ID_LIST_BITMAP, *__pBitmapComputer, null, null);
        break;

    case WIFI_DIRECT_DEVICE_TYPE_CATEGORY_INPUT_DEVICE:
        pItem->AddElement(Rectangle(BITMAP_ITEM_X, BITMAP_ITEM_Y, BITMAP_ITEM_WIDTH, BITMAP_ITEM_HEIGHT), ID_LIST_BITMAP, *__pBitmapInputDevice, null, null);
        break;

    default:
        pItem->AddElement(Rectangle(BITMAP_ITEM_X, BITMAP_ITEM_Y, BITMAP_ITEM_WIDTH, BITMAP_ITEM_HEIGHT), ID_LIST_BITMAP, *__pBitmapDefault, null, null);
        break;
    }


    static const int LIST_ITEM_X = 100;
    static const int LIST_ITEM_DEVICE_NAME_X = 5;
    static const int LIST_ITEM_DEVICE_NAME_HEIGHT = 50;
    static const int LIST_ITEM_IPADDRESS_X = 50;
    static const int LIST_ITEM_IPADDRESS_HEIGHT = 40;

    static const int ID_LIST_DEVICENAME = 101;
    static const int ID_LIST_ADDRESS = 102;


    pItem->AddElement(Rectangle(LIST_ITEM_X, LIST_ITEM_DEVICE_NAME_X, GetClientAreaBounds().width - LIST_ITEM_X - ListItemBase::GetAnnexWidth(LIST_ANNEX_STYLE_MARK), LIST_ITEM_DEVICE_NAME_HEIGHT), ID_LIST_DEVICENAME, deviceName, true);
    pItem->AddElement(Rectangle(LIST_ITEM_X, LIST_ITEM_IPADDRESS_X, GetClientAreaBounds().width - LIST_ITEM_X - ListItemBase::GetAnnexWidth(LIST_ANNEX_STYLE_MARK), LIST_ITEM_IPADDRESS_HEIGHT), ID_LIST_ADDRESS, displayAddress, true);
    return pItem;
}

bool
DeviceListForm::DeleteItem(int groupIndex, int itemIndex, ListItemBase* pItem, int itemWidth)
{
	delete pItem;
	return true;
}

bool
DeviceListForm::DeleteGroupItem(int groupIndex, GroupItem* pItem, int itemWidth)
{
	delete pItem;
	return true;
}

void
DeviceListForm::OnFormBackRequested(Form& source)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition());
}

void
DeviceListForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    bool ret = AddFooter();
    TryReturnVoid(ret == true, "Failed to add Footer");

    if(__pGroupedListView != null)
    {
        RemoveControl(*__pGroupedListView);
    }

    // create grouped list view
    __pGroupedListView = new GroupedListView;
    __pGroupedListView->Construct(Rectangle(0, 0, GetClientAreaBounds().width, GetClientAreaBounds().height), GROUPED_LIST_VIEW_STYLE_INDEXED);
    __pGroupedListView->AddGroupedListViewItemEventListener(*this);
    __pGroupedListView->SetItemProvider(*this);
    AddControl(*__pGroupedListView);

    TryReturnVoid(pArgs != null, "The argument list is null");
    if (__requestId != FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
       String* pString = static_cast<String*>(pArgs->GetAt(0));
       if (pString->Equals(String(L"true")))
       {
           __isGroupOwner = true;
       }

       pArgs->RemoveAt(0, true);

       __pDeviceInfoList.reset(pArgs);

       int deviceCount = __pDeviceInfoList->GetCount();

       for (int i = 0; i < deviceCount; i++)
       {
           WifiDirectDeviceInfo* pDeviceInfo = static_cast<WifiDirectDeviceInfo*>(__pDeviceInfoList->GetAt(i));

           if (pDeviceInfo->GetGroupMemberType() == WIFI_DIRECT_GROUP_MEMBER_TYPE_OWNER)
           {
               __groupOwnerCount++;
           }
       }
       __groupClientCount = deviceCount - __groupOwnerCount;
    }

    RequestRedraw();
}

bool
DeviceListForm::AddFooter(void)
{
    Footer* pFooter = GetFooter();
    TryReturn(pFooter != null, false, "Failed to get the Footer instance");

    pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);

    if (__requestId == FormFactory::REQUEST_SCAN_RESULT_DEVICE_LIST_FORM)
    {
        FooterItem footerItemScan;
        footerItemScan.Construct(ID_OPTION_SCAN);
        footerItemScan.SetText("Retry Scan");
        pFooter->AddItem(footerItemScan);
    }
    else if(__requestId == FormFactory::REQUEST_CHAT_MEMBER_DEVICE_LIST_FORM)
    {
        FooterItem footerItemChat;
        footerItemChat.Construct(ID_OPTION_CHAT);
        footerItemChat.SetText("Chat");
        pFooter->AddItem(footerItemChat);
    }
    else if(__requestId == FormFactory::REQUEST_PEER_DEVICE_LIST_FORM)
    {
        FooterItem footerItemDisconnect;
        footerItemDisconnect.Construct(ID_OPTION_DISCONNECT);
        footerItemDisconnect.SetText("Disconnect");
        pFooter->AddItem(footerItemDisconnect);
    }

    pFooter->AddActionEventListener(*this);
    pFooter->RequestRedraw();

    return true;
}

void
DeviceListForm::UpdateScanResult(Tizen::Base::Collection::IList *pScanResultList)
{
    __groupOwnerInfoList.RemoveAll();
    __othersInfoList.RemoveAll();

    __pDeviceInfoList.reset(pScanResultList);
    int deviceCount = __pDeviceInfoList->GetCount();

     for (int i = 0; i < deviceCount; i++)
     {
         WifiDirectDeviceInfo* pDeviceInfo = static_cast<WifiDirectDeviceInfo*>( __pDeviceInfoList->GetAt(i));

         if (pDeviceInfo->GetGroupMemberType() == WIFI_DIRECT_GROUP_MEMBER_TYPE_OWNER)
         {
             __groupOwnerInfoList.Add(*pDeviceInfo);
             __groupOwnerCount++;
         }
         else
         {
             __othersInfoList.Add(*pDeviceInfo);
         }
     }
     __groupClientCount = deviceCount - __groupOwnerCount;
     __pGroupedListView->UpdateList();


}
