/*
	VDMediator
	Mediator observable
*/
// TODO 

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// Osc
#include "VDOsc.h"
#include <memory>
#include <vector>
//#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
/*using namespace asio;
using namespace asio::ip;
using namespace ci::osc;*/

namespace videodromm {
	class VDUniformObserver;
	typedef std::shared_ptr<class VDUniformObserver> VDUniformObserverRef;

	class VDUniformObserver : public std::enable_shared_from_this<VDUniformObserver> {
	public:
		virtual VDUniformObserverRef setUniformValue(int aIndex, float aValue) = 0;
		/*VDUniformObserverRef getInstance() const {
			return std::shared_from_this();
		}*/
			
	private:
	};

	class VDMediatorObservable;
	typedef std::shared_ptr<class VDMediatorObservable> VDMediatorObservableRef;
	class VDMediatorObservable : public std::enable_shared_from_this<VDMediatorObservable> {
	public:
		static VDMediatorObservableRef createVDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			return VDMediatorObservableRef(new VDMediatorObservable(aVDSettings, aVDAnimation));
		}

		VDMediatorObservableRef addObserver(VDUniformObserverRef o) {
			mObservers.push_back(o);
			return shared_from_this();
		}
		float getUniformValue(int aIndex) {
			return mVDAnimation->getUniformValue(aIndex);
		}
		VDMediatorObservableRef setupOSCReceiver() {
			//mVDOsc
			mVDOsc = VDOsc::create(mVDSettings, mVDAnimation);
			mVDOsc->setupOSCReceiver();//shared_from_this()
			return shared_from_this();
		}
		
		VDMediatorObservableRef setUniformValue(int aIndex, float aValue) {
			for (auto observer : mObservers) {
				observer->setUniformValue(aIndex, aValue);
			}
			return shared_from_this();
		};

		VDMediatorObservableRef updateShaderText(int aIndex, float aValue) {
			for (auto observer : mObservers) {
				observer->setUniformValue(aIndex, aValue);
			}
			return shared_from_this();
		};
	private:
		std::vector<VDUniformObserverRef>	mObservers;
		// Settings
		VDSettingsRef						mVDSettings;
		// Animation
		VDAnimationRef						mVDAnimation;
		// OSC
		VDOscRef							mVDOsc;
		VDMediatorObservable() {}
		VDMediatorObservable::VDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {

			CI_LOG_V("VDMediatorObservable constructor");
			mVDSettings = aVDSettings;
			mVDAnimation = aVDAnimation;
		}
	};

}
		/*void update(void (*callback) (VDUniformObserverRef, map<string, any::type>)) {
			for (VDUniformObserverRef observer : mObservers) {
				callback(observer, params);
			}
		};*/
/*VDMediatorObservableRef mVDMediator = VDMediatorObservable::create()->addObserver(VDSocketIOObserverBuilder->connect(ip, port))->addObserver(new UIDisplay());

mVDMediator->setUniformValue(a, b);
mVDMediator->update([](observer, { "a": 1, "b" : 2 }) -> {
	observer->setUniformValue(a, b);
});

VDProxy::connect(ip, port)

*/
