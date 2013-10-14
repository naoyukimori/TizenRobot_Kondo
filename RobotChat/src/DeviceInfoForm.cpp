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

#include "DeviceInfoForm.h"
#include "FormFactory.h"

using namespace std;
using namespace Tizen::Base;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Net::Wifi;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::Ui::Scenes;

DeviceInfoForm::DeviceInfoForm(void)
	: __pGroupedListView(null)
    , __pLocalDeviceInfo(null)
{
}

DeviceInfoForm::~DeviceInfoForm(void)
{
}

bool
DeviceInfoForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL | FORM_STYLE_HEADER | FORM_STYLE_INDICATOR);

	SetName(FORM_DEVICE_INFO);

	return true;
}

result
DeviceInfoForm::OnInitializing(void)
{
	Header* pHeader = GetHeader();
	if (pHeader != null)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
		pHeader->SetTitleText(L"Local device info");
	}

	SetFormBackEventListener(this);

    __pGroupedListView = new GroupedListView;
    __pGroupedListView->Construct(Rectangle(0, 0, GetClientAreaBounds().width, GetClientAreaBounds().height), GROUPED_LIST_VIEW_STYLE_INDEXED);
    __pGroupedListView->SetItemProvider(*this);
    AddControl(*__pGroupedListView);

	return E_SUCCESS;
}

String
DeviceInfoForm::GetDeviceStatus(void)
{
	switch (__pLocalDeviceInfo->GetDeviceStatus())
	{
	case WIFI_DIRECT_DEVICE_ACTIVATED:
		return(L"Activated");

	case WIFI_DIRECT_DEVICE_DEACTIVATED:
		return(L"Deactivated");

	default:
		break;
	}
	return(L"");
}

String
DeviceInfoForm::GetGroupMemberType(void)
{
	switch (__pLocalDeviceInfo->GetGroupMemberType())
	{
	case WIFI_DIRECT_GROUP_MEMBER_TYPE_CLIENT:
		return(L"Group Client");

	case WIFI_DIRECT_GROUP_MEMBER_TYPE_OWNER:
		return(L"Group Owner");

	case WIFI_DIRECT_GROUP_MEMBER_TYPE_NONE:
		return(L"Non Member");

	default:
		break;
	}
	return(L"");
}

String
DeviceInfoForm::GetDeviceTypeCategory(void)
{
	switch (__pLocalDeviceInfo->GetDeviceTypeCategory())
	{
	case WIFI_DIRECT_DEVICE_TYPE_CATEGORY_TELEPHONE:
		return(L"Telephone");

	default:
		break;
	}
	return(L"Non Telephone");
}

String
DeviceInfoForm::GetDeviceWpsConfigurationMode(WifiWpsConfigurationMode wpsConfigurationMode)
{
    String wpsConfiguraitonMode;
	switch (wpsConfigurationMode)
	{
	case WIFI_WPS_CONFIG_MODE_PIN_DISPLAY:
	    wpsConfiguraitonMode = L"Pin Display";
	    break;
	case WIFI_WPS_CONFIG_MODE_PIN_KEYPAD:
	    wpsConfiguraitonMode = L"Pin Keypad";
	    break;
	case WIFI_WPS_CONFIG_MODE_PBC:
	    wpsConfiguraitonMode = L"Push Button Configuration";
	    break;
	default:
	    wpsConfiguraitonMode = L"Unknown";
		break;
	}
	return wpsConfiguraitonMode;
}

int
DeviceInfoForm::GetGroupCount(void)
{
    static const int NON_GROUP_MEMBER_GROUP_COUNT = 6;
    static const int GROUP_MEMBER_GROUP_COUNT = 8;

	if (__pLocalDeviceInfo->GetGroupMemberType() == WIFI_DIRECT_GROUP_MEMBER_TYPE_NONE)
	{
		return NON_GROUP_MEMBER_GROUP_COUNT;
	}
	else
	{
		return GROUP_MEMBER_GROUP_COUNT;
	}
}

int
DeviceInfoForm::GetItemCount(int groupIndex)
{
    return 1;
}

GroupItem*
DeviceInfoForm::CreateGroupItem(int groupIndex, int itemWidth)
{
    static const int GROUP_ITEM_HEIGHT = 50;

    static const wchar_t* pStringOfDeviceInfoType[DeviceInfoType::LENGTH] = { L"Device Name",
                                                                        L"MAC Address",
                                                                        L"IP Address",
                                                                        L"SSID",
                                                                        L"Device Status",
                                                                        L"Group Member Type",
                                                                        L"Device Category Type",
                                                                        L"WPS Configuration Modes" };

	GroupItem* pItem = new GroupItem();
	pItem->Construct(Dimension(GetClientAreaBounds().width, GROUP_ITEM_HEIGHT));

	// skip IP addrss and SSID
	if ((groupIndex >= DeviceInfoType::IP_ADDRESS) && (__pLocalDeviceInfo->GetGroupMemberType() == WIFI_DIRECT_GROUP_MEMBER_TYPE_NONE))
	{
		groupIndex += 2;
	}

	pItem->SetElement(pStringOfDeviceInfoType[groupIndex], null);
	return pItem;
}

ListItemBase*
DeviceInfoForm::CreateItem(int groupIndex, int itemIndex, int itemWidth)
{
    static const int LIST_ITEM_HEIGHT = 60;
    static const int ID_LIST_VALUE = 101;
    static const int LIST_DATA_HEIGHT = 50;
    static const int DEFAULT_MARGIN = 20;
    static const int ELEMENT_X = 26;
    static const int ELEMENT_Y = 0;

    int clientAreaWidth = GetClientAreaBounds().width;

    const IListT<WifiWpsConfigurationMode>* pWifiWpsConfigurationModes = null;

	CustomItem* pItem = new CustomItem();

	// skip IP addrss and SSID
	if ((groupIndex >= DeviceInfoType::IP_ADDRESS) && (__pLocalDeviceInfo->GetGroupMemberType() == WIFI_DIRECT_GROUP_MEMBER_TYPE_NONE))
	{
		groupIndex += 2;
	}

	if (groupIndex == DeviceInfoType::WPS_MODE)
    {
	    pWifiWpsConfigurationModes = static_cast<const IListT<WifiWpsConfigurationMode>*>(__pLocalDeviceInfo->GetSupportedWpsConfigurationModeList());
	    int wpsModeCount = pWifiWpsConfigurationModes->GetCount();

	    if (wpsModeCount == 0)
	    {
	        // set default value to display "Unknown"
	        wpsModeCount = 1;
	    }

        if (pWifiWpsConfigurationModes != null)
        {
        	pItem->Construct(Dimension(clientAreaWidth, ELEMENT_Y + (LIST_DATA_HEIGHT * wpsModeCount)), LIST_ANNEX_STYLE_NORMAL);

            for (int i = 0; i < wpsModeCount; i++)
            {
                WifiWpsConfigurationMode wpsConfigurationMode = WIFI_WPS_CONFIG_MODE_NONE;
                pWifiWpsConfigurationModes->GetAt(i, wpsConfigurationMode);
                pItem->AddElement(Rectangle(ELEMENT_X, ELEMENT_Y + (LIST_DATA_HEIGHT * i), clientAreaWidth - DEFAULT_MARGIN, LIST_DATA_HEIGHT), ID_LIST_VALUE + i,
                                  GetDeviceWpsConfigurationMode(wpsConfigurationMode));
            }
        }
    }
	else
    {
	    pItem->Construct(Tizen::Graphics::Dimension(GetClientAreaBounds().width, LIST_ITEM_HEIGHT), LIST_ANNEX_STYLE_NORMAL);

	    switch (groupIndex)
	    {
	    case DeviceInfoType::DEVICE_NAME:
	        pItem->AddElement(Rectangle(ELEMENT_X, ELEMENT_Y, clientAreaWidth - DEFAULT_MARGIN, LIST_DATA_HEIGHT), ID_LIST_VALUE,
	                          __pLocalDeviceInfo->GetDeviceName());
	        break;
	    case DeviceInfoType::MAC_ADDRESS:
	    {
	        String macAddress = __pLocalDeviceInfo->GetMacAddress();
	        if (macAddress.GetLength() == 0)
	        {
	            macAddress = L"xx-xx-xx-xx-xx-xx";
	        }

	        pItem->AddElement(Rectangle(ELEMENT_X, ELEMENT_Y, clientAreaWidth - DEFAULT_MARGIN, LIST_DATA_HEIGHT), ID_LIST_VALUE,
	                          macAddress);
	    }
	        break;
	    case DeviceInfoType::IP_ADDRESS:
	        if (__pLocalDeviceInfo->GetIpAddress() != null)
	        {
	            pItem->AddElement(Rectangle(ELEMENT_X, ELEMENT_Y, clientAreaWidth - DEFAULT_MARGIN, LIST_DATA_HEIGHT), ID_LIST_VALUE,
	                              __pLocalDeviceInfo->GetIpAddress()->ToString());
	        }
	        break;
	    case DeviceInfoType::SSID:
	        pItem->AddElement(Rectangle(ELEMENT_X, ELEMENT_Y, clientAreaWidth - DEFAULT_MARGIN, LIST_DATA_HEIGHT), ID_LIST_VALUE,
	                          __pLocalDeviceInfo->GetSsid());
	        break;

	    case DeviceInfoType::DEVICE_STATUS:
	        pItem->AddElement(Rectangle(ELEMENT_X, ELEMENT_Y, clientAreaWidth - DEFAULT_MARGIN, LIST_DATA_HEIGHT), ID_LIST_VALUE,
	                          GetDeviceStatus());
	        break;

	    case DeviceInfoType::MEMBER_TYPE:
	        pItem->AddElement(Rectangle(ELEMENT_X, ELEMENT_Y, clientAreaWidth - DEFAULT_MARGIN, LIST_DATA_HEIGHT), ID_LIST_VALUE,
	                          GetGroupMemberType());
	        break;

	    case DeviceInfoType::DEVICE_TYPE:
	        pItem->AddElement(Rectangle(ELEMENT_X, ELEMENT_Y, clientAreaWidth - DEFAULT_MARGIN, LIST_DATA_HEIGHT), ID_LIST_VALUE,
	                          GetDeviceTypeCategory());
	        break;
	    default:
	        break;
	    }
    }

	return pItem;
}

bool
DeviceInfoForm::DeleteItem(int groupIndex, int itemIndex, ListItemBase* pItem, int itemWidth)
{
	delete pItem;
	return true;
}

bool
DeviceInfoForm::DeleteGroupItem(int groupIndex, GroupItem* pItem, int itemWidth)
{
	delete pItem;
	return true;
}

void
DeviceInfoForm::OnFormBackRequested(Controls::Form& source)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition());
}

void
DeviceInfoForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    TryReturnVoid(pArgs != null, "The argument list is null");

    __pLocalDeviceInfo.reset(static_cast<WifiDirectDeviceInfo*>(pArgs->GetAt(0)));

    delete pArgs;

    RequestRedraw();
}
