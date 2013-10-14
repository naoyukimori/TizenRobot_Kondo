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

#ifndef _WPS_CONFIGURATION_MODE_FORM_H_
#define _WPS_CONFIGURATION_MODE_FORM_H_

#include <FApp.h>
#include <FBase.h>
#include <FUi.h>
#include <FNet.h>

class WpsConfigurationModeForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::IActionEventListener
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
{
public:
	WpsConfigurationModeForm(void);
	virtual ~WpsConfigurationModeForm(void);
	bool Initialize(void);

	virtual result OnInitializing(void);
	virtual void OnActionPerformed(const Tizen::Ui::Control& source, int actionId);

	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
									const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
									const Tizen::Ui::Scenes::SceneId &nextSceneId) {}

private:
	static const int ID_OPTION_DONE = 100;

	static const int CHECK_BUTTON_LEFT_MARGIN = 20;
	static const int CHECK_BUTTON_RIGHT_MARGIN = 40;
	static const int CHECK_BUTTON_HEIGHT = 112;
	static const int SCROLL_PANEL_HEIGHT = 300;

	Tizen::Ui::Controls::CheckButton* __pCheckButtonPinDisplay;
	Tizen::Ui::Controls::CheckButton* __pCheckButtonPinKeypad;
	Tizen::Ui::Controls::CheckButton* __pCheckButtonPushButton;
	Tizen::Ui::Controls::RadioGroup* __pRadioGroup;
};

#endif // _WPS_CONFIGURATION_MODE_FORM_H_
