/*
 * VideoForm.cpp
 *
 *  Created on: Oct 27, 2013
 *      Author: kien
 */

#include "VideoForm.h"
#include "MainForm.h"
#include "SceneRegister.h"
#include "FormFactory.h"

using namespace Tizen::Base;
using namespace Tizen::App;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base::Utility;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Media;

VideoForm::VideoForm()
	:	__pPlayer(null)
	,	__pOverlay(null)
{
}

VideoForm::~VideoForm(void)
{
	if (__pPlayer.get() != null)
	{
		PlayerState estate = __pPlayer->GetState();
		if ((estate == PLAYER_STATE_OPENED) || (estate == PLAYER_STATE_PAUSED)
			|| (estate == PLAYER_STATE_PLAYING)
			|| (estate == PLAYER_STATE_ENDOFCLIP) || (estate == PLAYER_STATE_STOPPED))
		{
			__pPlayer->Close();
		}
	}
}

bool
VideoForm::Initialize(void)
{
	VerticalBoxLayout formLayout;
	formLayout.Construct(VERTICAL_DIRECTION_DOWNWARD);

	Construct(formLayout, FORM_STYLE_NORMAL);
	SetName(FORM_ROBOT_CHAT);

	return true;
}

result
VideoForm::OnInitializing(void)
{
	result r = E_SUCCESS;
	SetOrientation(ORIENTATION_LANDSCAPE);

	SetFormBackEventListener(this);

	__pPlayer.reset(new (std::nothrow) Player());
	if ( __pPlayer.get() == null)
	{
		AppLogException("pPlayer = new (std::nothrow) Player() has failed");
		return E_FAILURE;
	}

	r = __pPlayer->Construct(*this, *this);
	if (IsFailed(r))
	{
		AppLogException("pPlayer: failed to construct the player");
		return r;
	}

	return E_SUCCESS;
}

void
VideoForm::OnFormBackRequested(Form& source)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition());
}

void
VideoForm::OnSceneActivatedN(const SceneId &previousSceneId, const SceneId &currentSceneId, IList *pArgs)
{
    TryReturnVoid(pArgs != null, "The argument list is null");
    String* fileName = static_cast<String*>(pArgs->GetAt(0));

    __pFilePath = Tizen::App::App::GetInstance()->GetAppResourcePath() + L"Video/Shopping/" + fileName->GetPointer();

    result r = PlayVideo();
    if (IsFailed(r))
    	SceneManager::GetInstance()->GoBackward(BackwardSceneTransition());
}

void
VideoForm::OnVideoFrameDecoded(Player &src, BitmapPixelFormat bitmapPixelFormat, const Dimension &dim,
										const byte *pBuffer, int sizeOfBuffer, result r)
{
    ByteBuffer buf;
    OverlayRegionBufferPixelFormat overlayPixelFormat;

    if (IsFailed(r))
    {
        return;
    }

    if (__pOverlay == null)
    {
        return;
    }

    if (bitmapPixelFormat == BITMAP_PIXEL_FORMAT_ARGB8888)
    {
        overlayPixelFormat = OVERLAY_REGION_BUFFER_PIXEL_FORMAT_ARGB8888;
    }
    else if (bitmapPixelFormat == BITMAP_PIXEL_FORMAT_RGB565)
    {
        overlayPixelFormat = OVERLAY_REGION_BUFFER_PIXEL_FORMAT_RGB565;
    }
    else // Unsupported pixel format
    {
        return;
    }

    buf.Construct(pBuffer, 0, sizeOfBuffer, sizeOfBuffer);
    __pOverlay->SetInputBuffer(buf, dim, overlayPixelFormat);
}

void
VideoForm::OnPlayerEndOfClip(void)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);

	pSceneManager->GoBackward(BackwardSceneTransition());
}

result
VideoForm::PlayVideo(void)
{
	result r = E_SUCCESS;

	r = __pPlayer->OpenFile(__pFilePath);
	if (IsFailed(r))
	{
		AppLogException("[DEBUG] Failed to open file");
		return r;
	}

	Rectangle rect = GetClientAreaBounds();
	__pOverlay.reset(Tizen::Ui::Controls::Form::GetOverlayRegionN(rect, OVERLAY_REGION_TYPE_NORMAL));
	if (__pOverlay == null)
	{
		return E_FAILURE;
	}

	r = __pPlayer->Play();
	if (IsFailed(r))
	{
		return r;
	}

	return r;
}
