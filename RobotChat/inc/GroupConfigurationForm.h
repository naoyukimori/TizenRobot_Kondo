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

#ifndef _GROUP_COFIGURATION_FORM_H_
#define _GROUP_COFIGURATION_FORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FGraphics.h>
#include <FNet.h>

class GroupConfigurationForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::ITextEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
{

public:
	GroupConfigurationForm(void);
	virtual ~GroupConfigurationForm(void);
	bool Initialize(void);

	virtual result OnInitializing(void);
	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);

	virtual void OnTextValueChanged(const Tizen::Ui::Control& source);
	virtual void OnTextValueChangeCanceled(const Tizen::Ui::Control& source) {};

	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
									const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
									const Tizen::Ui::Scenes::SceneId &nextSceneId) {}

private:
	void ShowMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text);

private:
	static const int ID_OPTION_DONE = 100;

	static const int MESSAGE_BOX_TIMEOUT = 2000;

	static const int EDIT_FILED_X = 20;
	static const int EDIT_FIEILD_MARGIN = 40;
	static const int EDIT_FIELD_HEIGHT = 112;

	static const int EDIT_FIELD_GROUP_INDENT_LIMIT = 2;
	static const int EDIT_FIELD_GROUP_CLIENT_LIMIT = 1;

	Tizen::Ui::Controls::EditField* __pEditFieldIntent;
	Tizen::Ui::Controls::EditField* __pEditFieldClientCount;
};

#endif // _GROUP_COFIGURATION_FORM_H_
