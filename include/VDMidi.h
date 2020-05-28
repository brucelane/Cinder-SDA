/*
	VDMidi

*/

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Json.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDMidi instance
	typedef std::shared_ptr<class VDMidi> VDMidiRef;
	class VDMidi {
	public:
		VDMidi(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		static VDMidiRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDMidi>(new VDMidi(aVDSettings, aVDAnimation));
		}


	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
	};
}

