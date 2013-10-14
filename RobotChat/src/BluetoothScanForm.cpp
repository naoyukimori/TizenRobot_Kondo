/*
 * BluetoothScanForm.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: kien
 */
#include "BluetoothScanForm.h"
#include "FormFactory.h"
#include "MainForm.h"

using namespace std;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Net::Bluetooth;

BluetoothScanForm::BluetoothScanForm()
{

}

BluetoothScanForm::~BluetoothScanForm(void)
{

}

bool
BluetoothScanForm::Initialize(void)
{
	Construct(FORM_STYLE_NORMAL | FORM_STYLE_HEADER | FORM_STYLE_FOOTER);

	SetName(FORM_BLUETOOTH_SCAN);

	Header* pHeader = GetHeader();
	if (pHeader != null)
	{
		pHeader->SetStyle(HEADER_STYLE_TITLE);
		pHeader->SetTitleText(L"Bluetooth Scan");
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

	if (IsFailed(__btManager.Construct(*this)))
	{
		//AppLog("BluetoothManager::Construct() [Error].");
		return false;
	}

	if (!__btManager.IsActivated())
	{
		ShowTimeoutMessageBox(L"Bluetooth Scan", L"Please Activate Bluetooth", 3000);
		return false;
	}

	__btManager.SetBluetoothDeviceListener(this);

	SetFormBackEventListener(this);

	return true;
}

result
BluetoothScanForm::OnInitializing(void)
{

	return E_SUCCESS;
}

void
BluetoothScanForm::OnActionPerformed(const Control& source, int actionId)
{
	switch (actionId)
	{
	case ID_OPTION_DONE:
	{
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
BluetoothScanForm::ShowTimeoutMessageBox(const Tizen::Base::String& title, const Tizen::Base::String& text, unsigned long timeout)
{
	MessageBox messageBox;
	int result = 0;
	messageBox.Construct(title, text, MSGBOX_STYLE_NONE, timeout);
	messageBox.ShowAndWait(result);
}

void
BluetoothScanForm::OnFormBackRequested(Form& source)
{
    SceneManager* pSceneManager = SceneManager::GetInstance();
    AppAssert(pSceneManager);

    pSceneManager->GoBackward(BackwardSceneTransition());
}

void
BluetoothScanForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{

}

void
BluetoothScanForm::OnBluetoothDiscoveryDone(bool isCompleted)
{

}

void
BluetoothScanForm::OnBluetoothDiscoveryStarted(result r)
{

}

void
BluetoothScanForm::OnBluetoothRemoteDeviceFoundN(Tizen::Net::Bluetooth::BluetoothDevice* pFoundDevice)
{

}
