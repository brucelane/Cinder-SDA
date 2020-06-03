/*
	VDOscReceiver
	observable
*/
// TODO 

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Mediator
#include "VDMediator.h"

#include <memory>
#include <vector>
#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
using namespace asio;
using namespace asio::ip;
using namespace ci::osc;

namespace videodromm {
	/*namespace osc {
		typedef std::shared_ptr<SenderUdp> SenderUdpRef;
	}// subject: observer observable VDOscObservable
	*/
	class VDOscObservable;
	typedef std::shared_ptr<class VDOscObservable> VDOscObservableRef;

	class VDOscObservable : public std::enable_shared_from_this<VDOscObservable> {
	public:

		static VDOscObservableRef createVDOscObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			//mOscReceiver = std::make_shared<osc::ReceiverUdp>(aVDSettings->mOSCReceiverPort);
			return VDOscObservableRef(new VDOscObservable(aVDSettings, aVDAnimation)/*,mOscReceiver*/);
		}

		VDOscObservableRef addObserver(VDUniformObserverRef o) {
			mObservers.push_back(o);
			return shared_from_this();
		}

		VDOscObservableRef updateUniformValue(int aIndex, float aValue) {
			for (auto observer : mObservers) {
				observer->setUniformValue(aIndex, aValue);
			}
			return shared_from_this();
		};

	private:
		std::vector<VDUniformObserverRef>	mObservers;
		// osc
		std::shared_ptr<osc::ReceiverUdp>	mOscReceiver;
		// Settings
		VDSettingsRef						mVDSettings;
		// Animation
		VDAnimationRef						mVDAnimation;
		VDOscObservable() {}
		VDOscObservable::VDOscObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) /*: mOscReceiver(aVDSettings->mOSCReceiverPort)*/ {
			CI_LOG_V("VDOscObservable constructor");
			mVDSettings = aVDSettings;
			mVDAnimation = aVDAnimation;
			mOscReceiver = std::make_shared<osc::ReceiverUdp>(mVDSettings->mOSCReceiverPort);
		}
	};
	
}
