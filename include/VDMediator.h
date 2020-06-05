/*
	VDMediator
	Mediator observable
*/
// TODO 

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
// Settings
//#include "VDSettings.h"
// Animation
//#include "VDAnimation.h"
// Osc
#include "VDOscReceiver.h"
#include <memory>
#include <vector>
//#include "cinder/osc/Osc.h"

using namespace ci;
using namespace ci::app;
/*using namespace asio;
using namespace asio::ip;
using namespace ci::osc;*/

namespace videodromm {
	class VDOscReceiver;
	typedef std::shared_ptr<VDOscReceiver> VDOscReceiverRef;

	class VDUniformObserver;
	typedef std::shared_ptr<class VDUniformObserver> VDUniformObserverRef;

	class VDUniformObserver : public std::enable_shared_from_this<VDUniformObserver> {
	public:
		virtual VDUniformObserverRef setUniformValue(int aIndex, float aValue) = 0;
		/*VDUniformObserverRef getInstance() const {return std::shared_from_this();}*/			
	private:
	};
	class VDMediatorObservable;
	typedef std::shared_ptr<class VDMediatorObservable> VDMediatorObservableRef;
	class VDMediatorObservable : public std::enable_shared_from_this<VDMediatorObservable> {
	public:
		static VDMediatorObservableRef createVDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		VDMediatorObservableRef addObserver(VDUniformObserverRef o);
		VDMediatorObservableRef setupOSCReceiver(/*VDSessionFacadeRef aVDSession*/);
		float getUniformValue(unsigned int aIndex);
		std::string getUniformName(unsigned int aIndex);
		VDMediatorObservableRef setUniformValue(int aIndex, float aValue);
		VDMediatorObservableRef updateShaderText(int aIndex, float aValue);
	private:
		std::vector<VDUniformObserverRef>	mObservers;
		// Settings
		VDSettingsRef						mVDSettings;
		// Animation
		VDAnimationRef						mVDAnimation;
		// OSC
		VDOscReceiverRef							mVDOscReceiver;
		VDMediatorObservable() {}
		VDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
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
