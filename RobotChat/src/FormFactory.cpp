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

#include "FormFactory.h"
#include "MainForm.h"
#include "GroupConfigurationForm.h"
#include "DeviceListForm.h"
#include "ChatForm.h"
#include "RobotChatForm.h"
#include "ChatPortConfigurationForm.h"
#include "DeviceInfoForm.h"
#include "WpsConfigurationModeForm.h"
#include "BluetoothScanForm.h"
#include "BluetoothDebugForm.h"

using namespace Tizen::Ui::Scenes;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Base;

// Definitions of extern.
const wchar_t* FORM_MAIN = L"Form_Main";
const wchar_t* FORM_DEVICE_INFO = L"Form_DeviceInfo";
const wchar_t* FORM_GROUP_CONFIG = L"Form_GroupConfiguration";
const wchar_t* FORM_SCAN_RESULT_DEVICE_LIST = L"Form_ScanResultDeviecList";
const wchar_t* FORM_PEER_DEVICE_LIST = L"Form_GroupMemberDeviecList";
const wchar_t* FORM_CHAT_MEMBER_DEVICE_LIST = L"Form_ChatMemberDeviceList";
const wchar_t* FORM_CHAT = L"Form_Chat";
const wchar_t* FORM_ROBOT_CHAT = L"Form_RobotChat";
const wchar_t* FORM_CHAT_PORT_CONFIGURATION = L"Form_ChatPortConfiguration";
const wchar_t* FORM_WPS_CONFIGURATION_MODE = L"Form_WpsConfigurationMode";
const wchar_t* FORM_BLUETOOTH_SCAN = L"Form_BluetoothScan";
const wchar_t* FORM_BLUETOOTH_DEBUG = L"Form_BluetoothDebug";

// This is used only for SetName()
const wchar_t* FORM_DEVICE_LIST = L"Form_DeviceList";


FormFactory::FormFactory(void)
{
}

FormFactory::~FormFactory(void)
{
}

Form*
FormFactory::CreateFormN(const String& formId, const SceneId& sceneId)
{
	Form* pNewForm = null;
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	if (formId == FORM_MAIN)
	{
		MainForm* pForm = new (std::nothrow) MainForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_DEVICE_INFO)
    {
        DeviceInfoForm* pForm = new (std::nothrow) DeviceInfoForm();
        pForm->Initialize();
        pSceneManager->AddSceneEventListener(sceneId, *pForm);
        pNewForm = pForm;
    }
	else if (formId == FORM_GROUP_CONFIG)
	{
		GroupConfigurationForm* pForm = new (std::nothrow) GroupConfigurationForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_SCAN_RESULT_DEVICE_LIST)
	{
		DeviceListForm* pForm = new (std::nothrow) DeviceListForm(REQUEST_SCAN_RESULT_DEVICE_LIST_FORM);
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_PEER_DEVICE_LIST)
    {
        DeviceListForm* pForm = new (std::nothrow) DeviceListForm(REQUEST_PEER_DEVICE_LIST_FORM);
        pForm->Initialize();
        pSceneManager->AddSceneEventListener(sceneId, *pForm);
        pNewForm = pForm;
    }
	else if (formId == FORM_CHAT_MEMBER_DEVICE_LIST)
	{
		DeviceListForm* pForm = new (std::nothrow) DeviceListForm(REQUEST_CHAT_MEMBER_DEVICE_LIST_FORM);
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_CHAT)
	{
		ChatForm* pForm = new (std::nothrow) ChatForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_ROBOT_CHAT)
	{
		RobotChatForm* pForm = new (std::nothrow) RobotChatForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_CHAT_PORT_CONFIGURATION)
	{
		ChatPortConfigurationForm* pForm = new (std::nothrow) ChatPortConfigurationForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_WPS_CONFIGURATION_MODE)
    {
        WpsConfigurationModeForm* pForm = new (std::nothrow) WpsConfigurationModeForm();
        pForm->Initialize();
        pSceneManager->AddSceneEventListener(sceneId, *pForm);
        pNewForm = pForm;
    }
	else if (formId == FORM_BLUETOOTH_SCAN)
	{
		BluetoothScanForm* pForm = new (std::nothrow) BluetoothScanForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}
	else if (formId == FORM_BLUETOOTH_DEBUG)
	{
		BluetoothDebugForm* pForm = new (std::nothrow) BluetoothDebugForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}

	return pNewForm;
}

