/*
	VDSocketIOObserver
	Mediator SocketIO Sender
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

namespace videodromm {
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
	
}
