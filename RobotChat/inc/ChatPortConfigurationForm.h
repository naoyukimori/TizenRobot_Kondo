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

#ifndef _CHAT_PORT_CONFIGURATION_FORM_H_
#define _CHAT_PORT_CONFIGURATION_FORM_H_

#include <FBase.h>
#include <FUi.h>

class ChatPortConfigurationForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::ITextEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
{

public:
	ChatPortConfigurationForm(void);
	virtual ~ChatPortConfigurationForm(void);
	bool Initialize(void);

	virtual result OnInitializing(void);

	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);

	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	virtual void OnTextValueChanged(const Tizen::Ui::Control& source);
	virtual void OnTextValueChangeCanceled(const Tizen::Ui::Control& source) {};

	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
									const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
									const Tizen::Ui::Scenes::SceneId &nextSceneId) {}

private:
	void ShowMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text);

private:
	static const int ID_OPTION_DONE = 100;
	static const int MESSAGE_BOX_TIMEOUT = 2000;

	static const int MAX_PORT = 65535;
	static const int MIN_PORT = 1024;

	Tizen::Ui::Controls::EditField* __pEditFieldChatPort;
	Tizen::Base::String* __pChatPortNumber;
};

#endif // _CHAT_PORT_CONFIGURATION_FORM_H_
