/*
 * VideoForm.h
 *
 *  Created on: Oct 27, 2013
 *      Author: kien
 */

#ifndef VIDEOFORM_H_
#define VIDEOFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>

using namespace Tizen::Base;
using namespace Tizen::Graphics;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Media;

class VideoForm
	: public Tizen::Ui::Controls::Form
	, public Tizen::Ui::Controls::IFormBackEventListener
	, public Tizen::Ui::Scenes::ISceneEventListener
	, public Tizen::Media::IPlayerEventListener
	, public Tizen::Media::IPlayerVideoEventListener
{
public:
	VideoForm();
	virtual ~VideoForm(void);

	bool Initialize(void);

	// Form
	virtual result OnInitializing(void);

	// IFormBackEventListener
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	// ISceneEventListener
	virtual void OnSceneActivatedN(const Tizen::Ui::Scenes::SceneId &previousSceneId,
	                                const Tizen::Ui::Scenes::SceneId &currentSceneId, Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(const Tizen::Ui::Scenes::SceneId &currentSceneId,
	                                const Tizen::Ui::Scenes::SceneId &nextSceneId){}

	// IPlayerEventListener
	virtual void OnPlayerOpened(result r){}
	virtual void OnPlayerEndOfClip(void);
	virtual void OnPlayerBuffering(int percent){}
	virtual void OnPlayerErrorOccurred(Tizen::Media::PlayerErrorReason r){}
	virtual void OnPlayerInterrupted(void){}
	virtual void OnPlayerReleased(void){}
	virtual void OnPlayerSeekCompleted(result r){}
	virtual void OnPlayerAudioFocusChanged(void){}

	// IPlayerVideoEventListener
	virtual void OnVideoFrameDecoded(Player &src, BitmapPixelFormat bitmapPixelFormat, const Dimension &dim,
	                                      const byte *pBuffer, int sizeOfBuffer, result r);

private:
	std::unique_ptr<Tizen::Media::Player> __pPlayer;
	std::unique_ptr<Tizen::Ui::Controls::OverlayRegion> __pOverlay;
	String __pFilePath;

	result PlayVideo(void);
};


#endif /* VIDEOFORM_H_ */
