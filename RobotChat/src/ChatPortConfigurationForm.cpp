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
#include "ChatPortConfigurationForm.h"
#include "FormFactory.h"

using namespace std;
using namespace Tizen::Base;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Base::Collection;
using namespace Tizen::Ui::Scenes;

ChatPortConfigurationForm::ChatPortConfigurationForm(void)
	: __pEditFieldChatPort(null)
	, __pChatPortNumber(null)
{
}

ChatPortConfigurationForm::~ChatPortConfigurationForm(void)
{
}

bool
ChatPortConfigurationForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL | FORM_STYLE_HEADER | FORM_STYLE_INDICATOR | FORM_STYLE_FOOTER);
	SetName(FORM_CHAT_PORT_CONFIGURATION);

	Header* pHeader = GetHeader();
	if (pHeader != null)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
		pHeader->SetTitleText(L"Chat port configuration");
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

	return true;
}

void
ChatPortConfigurationForm::ShowMessageBox(const String& title, const String& text)
{
	MessageBox messageBox;
	int modalResult = 0;

	messageBox.Construct(title, text, MSGBOX_STYLE_NONE, MESSAGE_BOX_TIMEOUT);
	messageBox.ShowAndWait(modalResult);
}

result
ChatPortConfigurationForm::OnInitializing(void)
{
	Rectangle clientRect = GetClientAreaBounds();

	const int EDIT_FIELD_HEIGHT = 112;
	const int EDIT_FIEILD_MARGIN = 40;
	const int EDIT_FILED_X = 20;
	const int EDIT_FILED_Y = (clientRect.height / 2) - EDIT_FIELD_HEIGHT;
	const int PORT_LIMIT = 5;

	__pEditFieldChatPort = new EditField;
	__pEditFieldChatPort->Construct(Rectangle(EDIT_FILED_X, EDIT_FILED_Y, clientRect.width - EDIT_FIEILD_MARGIN, EDIT_FIELD_HEIGHT),
									EDIT_FIELD_STYLE_NUMBER, INPUT_STYLE_FULLSCREEN, true, PORT_LIMIT, GROUP_STYLE_SINGLE);
	__pEditFieldChatPort->SetTitleText(L"Chat port number");
	__pEditFieldChatPort->AddTextEventListener(*this);
	AddControl(*__pEditFieldChatPort);

	Draw(true);

	return E_SUCCESS;
}

void
ChatPortConfigurationForm::OnActionPerformed(const Control& source, int actionId)
{
	switch (actionId)
	{
	case ID_OPTION_DONE:
	{
		if (__pEditFieldChatPort->GetTextLength() == 0)
		{
			__pEditFieldChatPort->SetText(*__pChatPortNumber);
			__pEditFieldChatPort->RequestRedraw();
			ShowMessageBox(L"Error", L"No Port Number");
			return;
		}

		int chatPortNumber = 0;
		Integer::Decode(__pEditFieldChatPort->GetText(), chatPortNumber);
		if (chatPortNumber < MIN_PORT || chatPortNumber > MAX_PORT)
		{
			__pEditFieldChatPort->SetText(*__pChatPortNumber);
			__pEditFieldChatPort->RequestRedraw();
			ShowMessageBox(L"Error", L"Invalid Port Number\nRange : 1024 - 65536");
			return;
		}

		__pChatPortNumber->Clear();
		__pChatPortNumber->Append(__pEditFieldChatPort->GetText());

		SceneManager* pSceneManager = SceneManager::GetInstance();
		AppAssert(pSceneManager);

		pSceneManager->GoBackward(BackwardSceneTransition());
	}
	break;

	default:
		break;
	}
}

void
ChatPortConfigurationForm::OnFormBackRequested(Form& source)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition());
}

void
ChatPortConfigurationForm::OnTextValueChanged(const Control& source)
{
	SetFocus();
}

void
ChatPortConfigurationForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    TryReturnVoid(pArgs != null, "The argument list is null");

    __pChatPortNumber = static_cast<String*>(pArgs->GetAt(0));
    __pEditFieldChatPort->SetText(*__pChatPortNumber);

    delete pArgs;

    RequestRedraw();
}
