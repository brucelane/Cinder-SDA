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
#include "sio_client.h"

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
		void update()
		{
			io.socket()->on("received", [&](sio::event& ev) {
				sio::message::ptr msg = ev.get_message();
				std::string str = msg->get_string();
			});
		}
		~VDSocketIOObserver() { /*socketIO.close();*/ }
	private:
		VDSocketIOObserver(std::string host, unsigned int port) {
			io.connect("http://127.0.0.1:3000");
			std::string s = "Socket.io";
			// emit text
			io.socket()->emit("test_text", sio::string_message::create(s));
		}
		sio::client io;
	};
	
}
