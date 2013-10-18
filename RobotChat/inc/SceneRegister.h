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

#ifndef _SCENE_REGISTER_H_
#define _SCENE_REGISTER_H_

// Use 'extern' to eliminate duplicate data allocation.
extern const wchar_t* SCENE_MAIN_FORM;
extern const wchar_t* SCENE_DEVICE_INFO_FORM;
extern const wchar_t* SCENE_GROUP_CONFIG_FORM;
extern const wchar_t* SCENE_SCAN_RESULT_DEVICE_LIST_FORM;
extern const wchar_t* SCENE_PEER_DEVICE_LIST_FORM;
extern const wchar_t* SCENE_CHAT_MEMBER_DEVICE_LIST_FORM;
extern const wchar_t* SCENE_CHAT_FORM;
extern const wchar_t* SCENE_ROBOT_CHAT_FORM;
extern const wchar_t* SCENE_CHAT_PORT_CONFIG_FORM;
extern const wchar_t* SCENE_WPS_CONFIG_MODE_FORM;
extern const wchar_t* SCENE_BLUETOOTH_SCAN_FORM;

class SceneRegister
{
public:
    static void RegisterAllScenes(void);

private:
    SceneRegister(void);
    ~SceneRegister(void);
};

#endif // _SCENE_REGISTER_H_
