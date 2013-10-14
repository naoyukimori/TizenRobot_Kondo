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

#include "SceneRegister.h"
#include "FormFactory.h"
#include <FUi.h>

using namespace Tizen::Ui::Scenes;

// Definitions of extern.
const wchar_t* SCENE_MAIN_FORM = L"Scene_MainForm";
const wchar_t* SCENE_DEVICE_INFO_FORM = L"Scene_DeviceInfoForm";
const wchar_t* SCENE_GROUP_CONFIG_FORM = L"Scene_GroupConfigurationForm";
const wchar_t* SCENE_SCAN_RESULT_DEVICE_LIST_FORM = L"Scene_ScanResultDeviceListForm";
const wchar_t* SCENE_PEER_DEVICE_LIST_FORM = L"Scene_GroupMemberDeviceListForm";
const wchar_t* SCENE_CHAT_MEMBER_DEVICE_LIST_FORM = L"Scene_ChatMemberDeviceListForm";
const wchar_t* SCENE_CHAT_FORM = L"Scene_ChatForm";
const wchar_t* SCENE_CHAT_PORT_CONFIG_FORM = L"Scene_ChatPortConfigurationForm";
const wchar_t* SCENE_WPS_CONFIG_MODE_FORM = L"Scene_WpsConfigurationModeForm";
const wchar_t* SCENE_BLUETOOTH_SCAN_FORM = L"Scene_BluetoothScanForm";

SceneRegister::SceneRegister(void)
{
}

SceneRegister::~SceneRegister(void)
{
}

void
SceneRegister::RegisterAllScenes(void)
{
    static const wchar_t* PANEL_BLANK = L"";
    static FormFactory formFactory;

    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);
    pSceneManager->RegisterFormFactory(formFactory);

    pSceneManager->RegisterScene(SCENE_MAIN_FORM, FORM_MAIN, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_DEVICE_INFO_FORM, FORM_DEVICE_INFO, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_GROUP_CONFIG_FORM, FORM_GROUP_CONFIG, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_SCAN_RESULT_DEVICE_LIST_FORM, FORM_SCAN_RESULT_DEVICE_LIST, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_PEER_DEVICE_LIST_FORM, FORM_PEER_DEVICE_LIST, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_CHAT_MEMBER_DEVICE_LIST_FORM, FORM_CHAT_MEMBER_DEVICE_LIST, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_CHAT_FORM, FORM_CHAT, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_CHAT_PORT_CONFIG_FORM, FORM_CHAT_PORT_CONFIGURATION, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_WPS_CONFIG_MODE_FORM, FORM_WPS_CONFIGURATION_MODE, PANEL_BLANK);
    pSceneManager->RegisterScene(SCENE_BLUETOOTH_SCAN_FORM, FORM_BLUETOOTH_SCAN, PANEL_BLANK);
}
