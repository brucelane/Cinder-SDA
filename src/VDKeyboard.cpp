#include "VDKeyboard.h"

using namespace videodromm;

VDKeyboardRef VDKeyboard::create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
{
	return std::shared_ptr<VDKeyboard>(new VDKeyboard(aVDSettings, aVDAnimation));
}

VDKeyboard::VDKeyboard(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	CI_LOG_V("VDKeyboard constructor");
}

void VDKeyboard::setupKeyboard(VDMediatorObservableRef aVDMediator) {
	mVDMediator = aVDMediator;
}

bool VDKeyboard::handleKeyDown(KeyEvent& event) {

	bool handled = true;
	float newValue;
#if defined( CINDER_COCOA )
	bool isModDown = event.isMetaDown();
#else // windows
	bool isModDown = event.isControlDown();
#endif
	bool isShiftDown = event.isShiftDown();
	bool isAltDown = event.isAltDown();
	float delta = 0.0f;
	if (isShiftDown) { delta = 0.05f; };
	if (isModDown) { delta = -0.05f; };
	CI_LOG_V("VDKeyboard keydown: " + toString(event.getCode()) + " ctrl: " + toString(isModDown) + " shift: " + toString(isShiftDown) + " alt: " + toString(isAltDown));
	// pass this key event to the warp editor first

	switch (event.getCode()) {

	case KeyEvent::KEY_x:
		// trixels
		mVDMediator->setUniformValue(mVDSettings->ITRIXELS, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->ITRIXELS) + delta, 0.0f));
		break;
	case KeyEvent::KEY_r:
		if (isAltDown) {
			mVDMediator->setUniformValue(mVDSettings->IBR, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IBR) + delta, 0.0f));
		}
		else {
			mVDMediator->setUniformValue(mVDSettings->IFR, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IFR) + delta, 0.0f));
		}
		break;
	case KeyEvent::KEY_g:
		if (isAltDown) {
			mVDMediator->setUniformValue(mVDSettings->IBG, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IBG) + delta, 0.0f));
		}
		else {
			mVDMediator->setUniformValue(mVDSettings->IFG, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IFG) + delta, 0.0f));
		}
		break;
	case KeyEvent::KEY_b:
		if (isAltDown) {
			mVDMediator->setUniformValue(mVDSettings->IBB, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IBB) + delta, 0.0f));
		}
		else {
			mVDMediator->setUniformValue(mVDSettings->IFB, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IFB) + delta, 0.0f));
		}
		break;
	case KeyEvent::KEY_a:
		mVDMediator->setUniformValue(mVDSettings->IFA, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IFA) + delta, 0.0f));
		break;
	case KeyEvent::KEY_u:
		// chromatic
		// TODO find why can't put value >0.9 or 0.85!
		newValue = math<float>::max(mVDAnimation->getUniformValue(mVDSettings->ICHROMATIC) + delta, 0.0f);
		if (newValue > 1.0f) newValue = 0.0f;
		mVDMediator->setUniformValue(mVDSettings->ICHROMATIC, newValue);
		break;
	case KeyEvent::KEY_p:
		// pixelate
		mVDMediator->setUniformValue(mVDSettings->IPIXELATE, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IPIXELATE) + delta, 0.0f));
		break;
	case KeyEvent::KEY_y:
		// glitch
		mVDMediator->setUniformValue(mVDSettings->IGLITCH, isModDown);
		break;
	case KeyEvent::KEY_i:
		// invert
		mVDMediator->setUniformValue(mVDSettings->IINVERT, isModDown);
		break;
	case KeyEvent::KEY_o:
		// toggle
		mVDMediator->setUniformValue(mVDSettings->ITOGGLE, isModDown);
		break;
	case KeyEvent::KEY_z:
		// zoom
		mVDMediator->setUniformValue(mVDSettings->IZOOM, math<float>::max(mVDAnimation->getUniformValue(mVDSettings->IZOOM) + delta, 0.0f));
		break;
		/* removed temp for Sky Project case KeyEvent::KEY_LEFT:
			//mVDTextures->rewindMovie();
			if (mVDAnimation->getUniformValue(21) > 0.1f) mVDSocketio->changeFloatValue(21, mVDAnimation->getUniformValue(21) - 0.1f);
			break;
		case KeyEvent::KEY_RIGHT:
			//mVDTextures->fastforwardMovie();
			if (mVDAnimation->getUniformValue(21) < 1.0f) mVDSocketio->changeFloatValue(21, mVDAnimation->getUniformValue(21) + 0.1f);
			break;*/
	case KeyEvent::KEY_PAGEDOWN:
	case KeyEvent::KEY_RIGHT:
		// crossfade right
		if (mVDAnimation->getUniformValue(mVDSettings->IXFADE) < 1.0f) {
			mVDMediator->setUniformValue(mVDSettings->IXFADE, mVDAnimation->getUniformValue(mVDSettings->IXFADE) + 0.01f);
		};
		break;
	case KeyEvent::KEY_PAGEUP:
	case KeyEvent::KEY_LEFT:
		// crossfade left
		if (mVDAnimation->getUniformValue(mVDSettings->IXFADE) > 0.0f) {
			mVDMediator->setUniformValue(mVDSettings->IXFADE, mVDAnimation->getUniformValue(mVDSettings->IXFADE) - 0.01f);
		}
		break;
	case KeyEvent::KEY_UP:
		// imgseq next
		//incrementSequencePosition();
		break;
	case KeyEvent::KEY_DOWN:
		// imgseq next
		//decrementSequencePosition();
		break;
	case KeyEvent::KEY_v:
		//if (isModDown) fboFlipV(0);// TODO other indexes mVDSettings->mFlipV = !mVDSettings->mFlipV; useless?
		break;
	default:
		CI_LOG_V("session keydown: " + toString(event.getCode()));
		handled = false;
		break;
	}

	CI_LOG_V((handled ? "VDKeyboard keydown handled " : "VDKeyboard keydown not handled "));
	event.setHandled(handled);
	return event.isHandled();
}

