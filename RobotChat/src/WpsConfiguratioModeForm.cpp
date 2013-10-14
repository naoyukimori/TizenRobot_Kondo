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
#include "WpsConfigurationModeForm.h"
#include "MainForm.h"
#include "FormFactory.h"

using namespace std;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Ui;
using namespace Tizen::Net::Wifi;
using namespace Tizen::Ui::Scenes;

WpsConfigurationModeForm::WpsConfigurationModeForm(void)
    : __pCheckButtonPinDisplay(null)
    , __pCheckButtonPinKeypad(null)
    , __pCheckButtonPushButton(null)
    , __pRadioGroup(null)
{
}

WpsConfigurationModeForm::~WpsConfigurationModeForm(void)
{
}

bool
WpsConfigurationModeForm::Initialize(void)
{
    Construct(FORM_STYLE_NORMAL | FORM_STYLE_HEADER | FORM_STYLE_INDICATOR | FORM_STYLE_FOOTER);

    SetName(FORM_WPS_CONFIGURATION_MODE);

    Header* pHeader = GetHeader();
    if (pHeader != null)
    {
        pHeader->SetStyle(HEADER_STYLE_TITLE);
        pHeader->SetTitleText(L"Preferred WPS mode");
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

result
WpsConfigurationModeForm::OnInitializing(void)
{
    Rectangle clientRect = GetClientAreaBounds();

    int currentHeight = (clientRect.height - (3 * CHECK_BUTTON_HEIGHT)) / 2;
    result r = E_SUCCESS;

    __pCheckButtonPinDisplay = new CheckButton;
    __pCheckButtonPinDisplay->Construct(Rectangle(CHECK_BUTTON_LEFT_MARGIN, currentHeight, clientRect.width - CHECK_BUTTON_RIGHT_MARGIN, CHECK_BUTTON_HEIGHT),
                                        CHECK_BUTTON_STYLE_RADIO, BACKGROUND_STYLE_DEFAULT, false, L"PIN Display", GROUP_STYLE_NONE);

    __pCheckButtonPinDisplay->AddActionEventListener(*this);
    AddControl(*__pCheckButtonPinDisplay);

    currentHeight += CHECK_BUTTON_HEIGHT;

    __pCheckButtonPinKeypad = new CheckButton;
    __pCheckButtonPinKeypad->Construct(Rectangle(CHECK_BUTTON_LEFT_MARGIN, currentHeight, clientRect.width - CHECK_BUTTON_RIGHT_MARGIN, CHECK_BUTTON_HEIGHT),
                                       CHECK_BUTTON_STYLE_RADIO, BACKGROUND_STYLE_DEFAULT, false, L"PIN Keypad", GROUP_STYLE_NONE);
    __pCheckButtonPinKeypad->AddActionEventListener(*this);
    AddControl(*__pCheckButtonPinKeypad);

    currentHeight += CHECK_BUTTON_HEIGHT;

    __pCheckButtonPushButton = new CheckButton;
    __pCheckButtonPushButton->Construct(Rectangle(CHECK_BUTTON_LEFT_MARGIN, currentHeight, clientRect.width - CHECK_BUTTON_RIGHT_MARGIN, CHECK_BUTTON_HEIGHT),
                                        CHECK_BUTTON_STYLE_RADIO, BACKGROUND_STYLE_DEFAULT, false, L"Push Button Configuration", GROUP_STYLE_NONE);
    __pCheckButtonPushButton->AddActionEventListener(*this);
    AddControl(*__pCheckButtonPushButton);

    __pRadioGroup = new RadioGroup();
    __pRadioGroup->Construct();
    __pRadioGroup->Add(*__pCheckButtonPinDisplay);
    __pRadioGroup->Add(*__pCheckButtonPinKeypad);
    __pRadioGroup->Add(*__pCheckButtonPushButton);
    AddControl(*__pRadioGroup);

    Draw(true);

    return r;
}

void
WpsConfigurationModeForm::OnActionPerformed(const Control& source, int actionId)
{
    switch (actionId)
    {
    case ID_OPTION_DONE:
    {
        WifiWpsConfigurationMode wpsConfigurationMode;

        if (__pCheckButtonPinDisplay->IsSelected())
        {
            wpsConfigurationMode = WIFI_WPS_CONFIG_MODE_PIN_DISPLAY;
        }
        else if (__pCheckButtonPinKeypad->IsSelected())
        {
            wpsConfigurationMode = WIFI_WPS_CONFIG_MODE_PIN_KEYPAD;
        }
        else if (__pCheckButtonPushButton->IsSelected())
        {
            wpsConfigurationMode = WIFI_WPS_CONFIG_MODE_PBC;
        }
        SceneManager* pSceneManager = SceneManager::GetInstance();
        AppAssert(pSceneManager);

        unique_ptr<ArrayList> pArgs(new ArrayList());
        String* pString = new String();

        pString->Format(2, L"%1d", wpsConfigurationMode);
        pArgs->Add(*pString);

        pSceneManager->GoBackward(BackwardSceneTransition(), pArgs.release());
    }
    break;

    default:
        break;
    }
}

void
WpsConfigurationModeForm::OnFormBackRequested(Form& source)
{
    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    pSceneManager->GoBackward(BackwardSceneTransition());
}

void
WpsConfigurationModeForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    TryReturnVoid(pArgs != null, "The argument list is null");

    String* pString = static_cast<String*>(pArgs->GetAt(0));
    WifiWpsConfigurationMode wpsConfigurationMode;
    Integer::Decode(*pString, (int&)wpsConfigurationMode);
    delete pArgs;

    if (wpsConfigurationMode == WIFI_WPS_CONFIG_MODE_PIN_DISPLAY)
    {
        __pRadioGroup->SetSelectedItem(*__pCheckButtonPinDisplay);
    }
    else if (wpsConfigurationMode == WIFI_WPS_CONFIG_MODE_PIN_KEYPAD)
    {
        __pRadioGroup->SetSelectedItem(*__pCheckButtonPinKeypad);
    }
    else if (wpsConfigurationMode == WIFI_WPS_CONFIG_MODE_PBC)
    {
        __pRadioGroup->SetSelectedItem(*__pCheckButtonPushButton);
    }

    RequestRedraw();
}
