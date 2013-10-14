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

#include "WifiDirectChat.h"
#include "SceneRegister.h"

using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::System;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;

WifiDirectChat::WifiDirectChat(void)
{
}

WifiDirectChat::~WifiDirectChat(void)
{
}

Application*
WifiDirectChat::CreateInstance(void)
{
	// Create the instance through the constructor.
	return new (std::nothrow) WifiDirectChat();
}

bool
WifiDirectChat::OnAppInitializing(AppRegistry& appRegistry)
{
	static const wchar_t* APP_NAME = L"WifiDirectChat";

	// Create a Frame.
	Frame* pWifiDirectChatAppFrame = new Frame();
	pWifiDirectChatAppFrame->Construct();
	pWifiDirectChatAppFrame->SetName(APP_NAME);
	AddFrame(*pWifiDirectChatAppFrame);

	// Register all scenes.
	SceneRegister::RegisterAllScenes();
	SceneManager* pSceneManager = SceneManager::GetInstance();

	// Go to the first scene.
	result r = pSceneManager->GoForward(ForwardSceneTransition(SCENE_MAIN_FORM));
	TryReturn(!IsFailed(r), false, "%s", GetErrorMessage(r));

	return true;
}

bool
WifiDirectChat::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
    // TODO:
    // Deallocate resources allocated by this application.
    // The application's permanent data and context can be saved via appRegistry.

	return true;
}

void
WifiDirectChat::OnForeground(void)
{
    //TODO:
    // Add code to execute when the application is moved to the foreground.
}

void
WifiDirectChat::OnBackground(void)
{
    //TODO:
    // Add code to execute when the application is moved to the backgorund.

}

void
WifiDirectChat::OnLowMemory(void)
{
    // TODO:
    // Free unused resources or close the application.
}

void
WifiDirectChat::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
    // TODO:
    // Handle any changes in battery level here.
    // It is recommended that the application is closed if the battery level is low.
}

void
WifiDirectChat::OnScreenOn(void)
{
    // TODO:
    // Get the released resources or resume the operations that were paused or stopped in OnScreenOff().
}

void
WifiDirectChat::OnScreenOff(void)
{
    // TODO:
    //  Unless there is a strong reason to do otherwise, release resources (such as 3D, media, and sensors) to allow the device to enter the sleep mode to save the battery.
    // Invoking a lengthy asynchronous method within this listener method can be risky, because it is not guaranteed to invoke a callback before the device enters the sleep mode.
    // Similarly, do not perform lengthy operations in this listener method. Any operation must be a quick one.
}
