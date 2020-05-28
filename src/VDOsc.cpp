#include "VDOsc.h"

using namespace videodromm;

VDOsc::VDOsc(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;

	CI_LOG_V("VDOsc constructor");

}

