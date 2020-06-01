/*
	VDProxy
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
#include <memory>
#include <vector>

/*
1 fois
connect(ip, port)
plusieurs
changeUniform->sendMessage*/

using namespace ci;
using namespace ci::app;
//using namespace std;
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

	class VDUIDisplayObserver : public VDUniformObserver {

		virtual VDUniformObserverRef setUniformValue(int aIndex, float aValue) {
			//cout << a << b << endl;
			return shared_from_this();
		};
	};

	class VDSocketIOObserver : public VDUniformObserver {
	public:
		static VDUniformObserverRef connect(std::string host, unsigned int port) {
			//msocketio->sendMessage(a, b);
			VDUniformObserverRef obs(new VDSocketIOObserver(host, port));
			return obs;
		}
		VDUniformObserverRef setUniformValue(int aIndex, float aValue) {
			//msocketio->sendMessage(a, b);
			return shared_from_this();
		}

		~VDSocketIOObserver() { /*socketIO.close();*/ }
	private:
		VDSocketIOObserver(std::string host, unsigned int port) {
			//mclient.connect(host, port);
		}
		//sio::socketio mclient;
	};

	class VDOscObserver : public VDUniformObserver {
		VDUniformObserverRef connect(std::string host, unsigned int port) {
			//mosc->sendMessage(a, b);
			return shared_from_this();
		}
		VDUniformObserverRef setUniformValue(int aIndex, float aValue) {
			//mosc->sendMessage(a, b);
			return shared_from_this();
		}
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

		VDMediatorObservableRef updateUniformValue(int aIndex, float aValue) {
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

		/*void update(void (*callback) (VDUniformObserverRef, map<string, any::type>)) {
			for (VDUniformObserverRef observer : mObservers) {
				callback(observer, params);
			}
		};*/


	private:
		std::vector<VDUniformObserverRef> mObservers;
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;
		VDMediatorObservable() {}
		VDMediatorObservable::VDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {

			CI_LOG_V("VDMediatorObservable constructor");
			mVDSettings = aVDSettings;
			mVDAnimation = aVDAnimation;
		}
	};

}

	/*VDMediatorObservableRef mVDMediator = VDMediatorObservable::create()->addObserver(VDSocketIOObserverBuilder->connect(ip, port))->addObserver(new UIDisplay());

	mVDMediator->updateUniformValue(a, b);
	mVDMediator->update([](observer, { "a": 1, "b" : 2 }) -> {
		observer->setUniformValue(a, b);
	});

	VDProxy::connect(ip, port)
	
	*/
