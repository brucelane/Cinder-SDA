#include "VDMidi.h"

using namespace videodromm;

VDMidi::VDMidi(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;

	CI_LOG_V("VDMidi constructor");

}

