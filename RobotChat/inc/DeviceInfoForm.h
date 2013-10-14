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

#ifndef _DEVICE_INFO_FORM_H_
#define _DEVICE_INFO_FORM_H_

#include <unique_ptr.h>
#include <FBase.h>
#include <FUi.h>
#include <FNet.h>

namespace DeviceInfoType {
    enum Enum { DEVICE_NAME,
                MAC_ADDRESS,
                IP_ADDRESS,
                SSID,
                DEVICE_STATUS,
                MEMBER_TYPE,
                DEVICE_TYPE,
                WPS_MODE,
                LENGTH};
}

class DeviceInfoForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::Controls::IGroupedListViewItemProvider
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
{

public:
	DeviceInfoForm(void);
	virtual ~DeviceInfoForm(void);
	bool Initialize(void);

	result OnInitializing(void);

    // IGroupedListViewItemProvider
	virtual int GetGroupCount(void);
	virtual int GetItemCount(int groupIndex);
	virtual Tizen::Ui::Controls::GroupItem* CreateGroupItem(int groupIndex, int itemWidth);
	virtual bool DeleteGroupItem(int groupIndex, Tizen::Ui::Controls::GroupItem* pItem, int itemWidth);
	virtual Tizen::Ui::Controls::ListItemBase* CreateItem(int groupIndex, int itemIndex, int itemWidth);
	virtual bool DeleteItem(int groupIndex, int itemIndex, Tizen::Ui::Controls::ListItemBase* pItem, int itemWidth);

	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
									const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
									const Tizen::Ui::Scenes::SceneId &nextSceneId) {}

private:
	Tizen::Base::String GetDeviceStatus(void);
	Tizen::Base::String GetGroupMemberType(void);
	Tizen::Base::String GetDeviceTypeCategory(void);
	Tizen::Base::String GetDeviceWpsConfigurationMode(Tizen::Net::Wifi::WifiWpsConfigurationMode wpsConfigurationMode);

	Tizen::Ui::Controls::GroupedListView* __pGroupedListView;
	std::unique_ptr<Tizen::Net::Wifi::WifiDirectDeviceInfo> __pLocalDeviceInfo;
};

#endif // _DEVICE_INFO_FORM_H_
