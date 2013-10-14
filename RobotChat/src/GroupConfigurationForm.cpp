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

#include <unique_ptr.h>
#include "GroupConfigurationForm.h"
#include "MainForm.h"
#include "FormFactory.h"

using namespace std;
using namespace Tizen::Base;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Base::Collection;
using namespace Tizen::Net::Wifi;
using namespace Tizen::Ui::Scenes;

GroupConfigurationForm::GroupConfigurationForm(void)
	: __pEditFieldIntent(null)
	, __pEditFieldClientCount(null)
{
}

GroupConfigurationForm::~GroupConfigurationForm(void)
{
}

bool
GroupConfigurationForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL | FORM_STYLE_HEADER | FORM_STYLE_INDICATOR | FORM_STYLE_FOOTER);

	SetName(FORM_GROUP_CONFIG);

	return true;
}

void
GroupConfigurationForm::ShowMessageBox(const String& title, const String& text)
{
	MessageBox messageBox;
	int modalResult = 0;

	messageBox.Construct(title, text, MSGBOX_STYLE_NONE, MESSAGE_BOX_TIMEOUT);
	messageBox.ShowAndWait(modalResult);
}

result
GroupConfigurationForm::OnInitializing(void)
{
    result r = E_SUCCESS;

	Header* pHeader = GetHeader();
	if (pHeader != null)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
		pHeader->SetTitleText(L"Group configuration info");
	}

	Footer* pFooter = GetFooter();
	if (pFooter != null)
	{
		pFooter->SetStyle(FOOTER_STYLE_BUTTON_TEXT);

		FooterItem footerItemDone;
		footerItemDone.Construct(ID_OPTION_DONE);
		footerItemDone.SetText("Done");
		pFooter->AddItem(footerItemDone);

		pFooter->AddActionEventListener(*this);
	}

	SetFormBackEventListener(this);

	Rectangle clientRect = GetClientAreaBounds();
	static const int GAP_BETWEEN_GROUPS = 32;
	int emptySpaceHeight = (clientRect.height - (2 * EDIT_FIELD_HEIGHT + GAP_BETWEEN_GROUPS)) / 2;
	int currentHeight = emptySpaceHeight;

	__pEditFieldIntent = new EditField;
	__pEditFieldIntent->Construct(Rectangle(EDIT_FILED_X, currentHeight, clientRect.width - EDIT_FIEILD_MARGIN, EDIT_FIELD_HEIGHT),
								  EDIT_FIELD_STYLE_NUMBER, INPUT_STYLE_FULLSCREEN, true, EDIT_FIELD_GROUP_INDENT_LIMIT, GROUP_STYLE_NONE);
	__pEditFieldIntent->SetTitleText(L"Intent");
	__pEditFieldIntent->AddTextEventListener(*this);
	AddControl(*__pEditFieldIntent);

	currentHeight += EDIT_FIELD_HEIGHT + GAP_BETWEEN_GROUPS;

	__pEditFieldClientCount = new EditField;
	__pEditFieldClientCount->Construct(Rectangle(EDIT_FILED_X, currentHeight, clientRect.width - EDIT_FIEILD_MARGIN, EDIT_FIELD_HEIGHT),
									   EDIT_FIELD_STYLE_NUMBER, INPUT_STYLE_FULLSCREEN, true, EDIT_FIELD_GROUP_CLIENT_LIMIT, GROUP_STYLE_NONE);
	__pEditFieldClientCount->SetTitleText(L"Max. Clients");
	__pEditFieldClientCount->AddTextEventListener(*this);
	AddControl(*__pEditFieldClientCount);

	Draw(true);

	return r;
}

void
GroupConfigurationForm::OnActionPerformed(const Control& source, int actionId)
{
    if (actionId == ID_OPTION_DONE)
    {
        int groupOwnerIntent = 0;
        int maxClientCount = 8;

        Integer::Decode(__pEditFieldIntent->GetText(), groupOwnerIntent);
        Integer::Decode(__pEditFieldClientCount->GetText(), maxClientCount);

        if (groupOwnerIntent > MAX_WIFI_DIRECT_GROUP_OWNER_INTENT)
        {
            ShowMessageBox(L"Invalid Value", L"Intent cannot be greater than 15");
            return;
        }

        if (maxClientCount > MAX_WIFI_DIRECT_CONNECTED_CLIENTS)
        {
            ShowMessageBox(L"Invalid Value", L"Connected Clients cannot be greater than 8");
            return;
        }

        WifiDirectGroupInfo* pGroupInfo = new WifiDirectGroupInfo;
        pGroupInfo->SetGroupOwnerIntent(groupOwnerIntent);
        pGroupInfo->SetMaxNumberOfClients(maxClientCount);

        ArrayList* pArgs = new ArrayList();
        pArgs->Add(*pGroupInfo);

        SceneManager* pSceneManager = SceneManager::GetInstance();
        AppAssert(pSceneManager);
        pSceneManager->GoBackward(BackwardSceneTransition(), pArgs);
	}
}


void
GroupConfigurationForm::OnFormBackRequested(Controls::Form& source)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition());
}


void
GroupConfigurationForm::OnTextValueChanged(const Control& source)
{
}

void
GroupConfigurationForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    TryReturnVoid(pArgs != null, "The argument list is null");

    unique_ptr<IList> pList(pArgs);
    WifiDirectGroupInfo* pGroupInfo = static_cast<WifiDirectGroupInfo*>(pList->GetAt(0));
    TryReturnVoid(pGroupInfo != null, "Failed to get Group Info");

    String groupOwnerIntent;
    groupOwnerIntent.Format(3, L"%2d", pGroupInfo->GetGroupOwnerIntent());
    groupOwnerIntent.Trim();
    __pEditFieldIntent->SetText(groupOwnerIntent);

    String maxClients;
    maxClients.Format(2, L"%1d", pGroupInfo->GetMaxNumberOfClients());
    __pEditFieldClientCount->SetText(maxClients);

    RequestRedraw();
}
