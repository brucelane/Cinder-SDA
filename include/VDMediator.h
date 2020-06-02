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
#include "cinder/osc/Osc.h"
using namespace asio;
using namespace asio::ip;
using namespace ci::osc;

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
			return shared_from_this();
		};
	};

	class VDSocketIOObserver : public VDUniformObserver {
	public:
		static VDUniformObserverRef connect(std::string host, unsigned int port) {
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

	/*namespace osc {
		typedef std::shared_ptr<SenderUdp> SenderUdpRef;
	}// subject: observer observable VDOscObservable
	*/
	/*class VDOscObservable {
		
		typedef std::shared_ptr<class VDOscObservable> VDOscObservableRef;
		class VDOscObservable : public std::enable_shared_from_this<VDOscObservable> {
		public:

			static VDOscObservableRef createVDOscObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
			{
				mVDOscReceiver
				return VDOscObservableRef(new VDOscObservable(aVDSettings, aVDAnimation));
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

			VDOscObservableRef updateShaderText(int aIndex, float aValue) {
				for (auto observer : mObservers) {
					observer->setUniformValue(aIndex, aValue);
				}
				return shared_from_this();
			};
		private:
			std::vector<VDUniformObserverRef> mObservers;
			mVDOScReceiver;
			// Settings
			VDSettingsRef				mVDSettings;
			// Animation
			VDAnimationRef				mVDAnimation;
			VDOscObservable() {}
			VDOscObservable::VDOscObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {
				CI_LOG_V("VDOscObservable constructor");
				mVDSettings = aVDSettings;
				mVDAnimation = aVDAnimation;
			}
		};*/
	class VDOscObserver : public VDUniformObserver {
	public:
		static VDUniformObserverRef connect(std::string host, unsigned int port) {
			VDOscObserver* o = new VDOscObserver(host, port);
			o->bind();
			
			VDUniformObserverRef obs(o);
			
			return obs;
		}
		VDUniformObserverRef setUniformValue(int aIndex, float aValue) {
			//mosc->sendMessage(a, b);
			return shared_from_this();
		}
		VDOscObserver* bind() {
			mSender.bind();
			return this;
		}
		~VDOscObserver() {};
	private:
		VDOscObserver(std::string host, unsigned int port) : mSender(10002, "127.0.0.1", 10003) {
			//mSocket = new udp::socket(App::get()->io_service(), udp::endpoint(udp::v4(), port));
			//mSender = new SenderUdp(mSocket, udp::endpoint(address_v4::broadcast(), 10005));
			//mSocket->set_option(asio::socket_base::broadcast(true));
			
		}
		//osc::UdpSocket	mSocket;
		osc::SenderUdp	mSender;
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
