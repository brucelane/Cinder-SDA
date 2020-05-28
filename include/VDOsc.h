/*
	VDOsc
	
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
	// stores the pointer to the VDOsc instance
	typedef std::shared_ptr<class VDOsc> VDOscRef;
	class VDOsc {
	public:
		VDOsc(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		static VDOscRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDOsc>(new VDOsc(aVDSettings, aVDAnimation));
		}
		
	
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
	};
}

