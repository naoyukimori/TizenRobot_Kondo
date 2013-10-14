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

#ifndef _FORM_FACTORY_H_
#define _FORM_FACTORY_H_

#include <FUi.h>

// Use 'extern' to eliminate duplicate data allocation.
extern const wchar_t* FORM_MAIN;
extern const wchar_t* FORM_DEVICE_INFO;
extern const wchar_t* FORM_GROUP_CONFIG;
extern const wchar_t* FORM_SCAN_RESULT_DEVICE_LIST;
extern const wchar_t* FORM_PEER_DEVICE_LIST;
extern const wchar_t* FORM_CHAT_MEMBER_DEVICE_LIST;
extern const wchar_t* FORM_CHAT;
extern const wchar_t* FORM_CHAT_PORT_CONFIGURATION;
extern const wchar_t* FORM_WPS_CONFIGURATION_MODE;

// This is used only for SetName()
extern const wchar_t* FORM_DEVICE_LIST;

class FormFactory
	: public Tizen::Ui::Scenes::IFormFactory
{
public:
	FormFactory();
	virtual ~FormFactory();

	virtual Tizen::Ui::Controls::Form* CreateFormN(const Tizen::Base::String& formId, const Tizen::Ui::Scenes::SceneId& sceneId);

public:
	static const RequestId REQUEST_SCAN_RESULT_DEVICE_LIST_FORM = 100;
	static const RequestId REQUEST_CHAT_MEMBER_DEVICE_LIST_FORM = 101;
	static const RequestId REQUEST_PEER_DEVICE_LIST_FORM = 102;
};

#endif // _FORM_FACTORY_H_
