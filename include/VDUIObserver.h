/*
	VDUIObserver
	Mediator UI Observer
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

using namespace ci;
using namespace ci::app;
using namespace asio;
using namespace asio::ip;
using namespace ci::osc;

namespace videodromm {
	
	class VDUIObserver : public VDUniformObserver {
	public:
		static VDUniformObserverRef connect(std::string host, unsigned int port) {
			VDUIObserver* o = new VDUIObserver(host, port);
			o->bind();

			VDUniformObserverRef obs(o);

			return obs;
		}
		VDUniformObserverRef setUniformValue(int aIndex, float aValue) {
			
			return shared_from_this();
		}
		VDUIObserver* bind() {
			try {
				// Bind the sender to the endpoint. This function may throw. The exception will contain asio::error_code information.
				mSender.bind();
				mIsConnected = true;
			}
			catch (const osc::Exception& ex) {
				CI_LOG_E("Error binding: " << ex.what() << " val: " << ex.value());
			}
			return this;
		}
		
		bool mIsConnected = false;
		~VDUIObserver() { mSender.close(); };
	private:
		VDUIObserver(std::string host, unsigned int port) : mSender(10002, host, port) {
		}
		osc::SenderUdp	mSender;
	};
}
