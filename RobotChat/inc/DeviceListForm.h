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

#ifndef _DEVICE_LIST_FORM_H_
#define _DEVICE_LIST_FORM_H_

#include <unique_ptr.h>
#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FNet.h>
#include <FMedia.h>
#include <FIo.h>
#include <FAppApp.h>

class DeviceListForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::Controls::IGroupedListViewItemEventListener
	, public Tizen::Ui::Controls::IGroupedListViewItemProvider
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
{

public:
	DeviceListForm(RequestId requestId);
	virtual ~DeviceListForm(void);
	bool Initialize(void);
	void UpdateScanResult(Tizen::Base::Collection::IList *pScanResultList);

	// Form
	result OnInitializing(void);

	// IActionEventListener
	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);

	// IGroupedListViewItemEventListener
    virtual void OnGroupedListViewItemSwept(Tizen::Ui::Controls::GroupedListView& listView, int groupIndex, int itemIndex, Tizen::Ui::Controls::SweepDirection direction) {};
	virtual void OnGroupedListViewItemStateChanged(Tizen::Ui::Controls::GroupedListView& listView, int groupIndex, int itemIndex, int elementId, Tizen::Ui::Controls::ListItemStatus state);
    virtual void OnGroupedListViewContextItemStateChanged(Tizen::Ui::Controls::GroupedListView& listView, int groupIndex, int itemIndex, int elementId, Tizen::Ui::Controls::ListContextItemStatus state) {};

    // IGroupedListViewItemProvider
	virtual int GetGroupCount(void);
	virtual int GetItemCount(int groupIndex);
	virtual Tizen::Ui::Controls::GroupItem* CreateGroupItem(int groupIndex, int itemWidth);
	virtual bool DeleteGroupItem(int groupIndex, Tizen::Ui::Controls::GroupItem* pItem, int itemWidth);
	virtual Tizen::Ui::Controls::ListItemBase* CreateItem(int groupIndex, int itemIndex, int itemWidth);
	virtual bool DeleteItem(int groupIndex, int itemIndex, Tizen::Ui::Controls::ListItemBase* pItem, int itemWidth);

	// // IFormBackEventListener
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	// ISceneEventListener
	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
									const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
									const Tizen::Ui::Scenes::SceneId &nextSceneId) {}
private:
	void ShowMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text);
	bool AddFooter(void);

private:
	static const int ID_OPTION_SCAN = 100;
	static const int ID_OPTION_CHAT = 101;
    static const int ID_OPTION_DISCONNECT = 102;

    static const int INDEX_GROUP_OWNER = 0;

	Tizen::Ui::Controls::GroupedListView* __pGroupedListView;
	std::unique_ptr<Tizen::Graphics::Bitmap> __pBitmapTelephone;
	std::unique_ptr<Tizen::Graphics::Bitmap> __pBitmapComputer;
	std::unique_ptr<Tizen::Graphics::Bitmap> __pBitmapInputDevice;
	std::unique_ptr<Tizen::Graphics::Bitmap> __pBitmapDefault;
	std::unique_ptr<Tizen::Base::Collection::IList> __pDeviceInfoList;


	Tizen::Base::Collection::ArrayList __groupOwnerInfoList;
	Tizen::Base::Collection::ArrayList __othersInfoList;

	int __selectedGroupIndex;
	int __selectedItemIndex;
	int __groupOwnerCount;
	int __groupClientCount;
	bool __isGroupOwner;

	RequestId __requestId;
};

#endif  // _DEVICE_INFO_LIST_FORM_H_
