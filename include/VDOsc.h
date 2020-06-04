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
// Mediator
//#include "VDMediator.h"
// Session
//#include "VDSessionFacade.h"
// OSC
#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
//using namespace std;
using namespace ci::osc;
using namespace asio;
using namespace asio::ip;
#define USE_UDP 1

#if USE_UDP
using Receiver = osc::ReceiverUdp;
using protocol = asio::ip::udp;
#else
using Receiver = osc::ReceiverTcp;
using protocol = asio::ip::tcp;
#endif
namespace videodromm
{
	// stores the pointer to the VDOsc instance
	typedef std::shared_ptr<class VDOsc> VDOscRef;
	class VDOsc {
	public:
		VDOsc(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		static VDOscRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			return std::shared_ptr<VDOsc>(new VDOsc(aVDSettings, aVDAnimation));
		}	
		void						setupOSCReceiver(/*VDSessionFacadeRef aVDSession*/);
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Animation
		VDAnimationRef					mVDAnimation;
		// VDSession
		//VDSessionFacadeRef				mVDSession;
		float							mBarStart = 0.0f;
		// osc
		std::shared_ptr<osc::ReceiverUdp>	mOscReceiver;
//std::map<uint64_t, protocol::endpoint>	mConnections;
	};
}

