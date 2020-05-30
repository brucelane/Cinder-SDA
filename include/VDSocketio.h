/*
	VDSocketio
	Handles websocket connection (client)
*/
// TODO websocket server?
// TODO support Socket.io?

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/Json.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"

// SocketIO
#include "sio_client.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDSocketio instance
	typedef std::shared_ptr<class VDSocketio> VDSocketioRef;
	class VDSocketio {
	public:
		VDSocketio(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		static VDSocketioRef	create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			return shared_ptr<VDSocketio>(new VDSocketio(aVDSettings, aVDAnimation));
		}
		void						update();
        // messages
		void						sendJSON(string params);
		void						updateParams(int iarg0, float farg1);
		// SocketIO
		//void						sioWriteBinary(const void *data, int size);
		void						sioWrite(std::string msg);
		void						sioConnect();
		//bool						isWsClientConnected() { return clientConnected; };
		// change a control value and update network clients
		void						changeFloatValue(unsigned int aControl, float aValue, bool forceSend = false, bool toggle = false, bool increase = false, bool decrease = false);
		void						changeIntValue(unsigned int aControl, int aValue);
		void						changeBoolValue(unsigned int aControl, bool aValue);
		void						toggleAuto(unsigned int aIndex);
		void						toggleValue(unsigned int aIndex);
		void						toggleTempo(unsigned int aIndex);
		void						resetAutoAnimation(unsigned int aIndex);
		void						changeShaderIndex(unsigned int aWarpIndex, unsigned int aWarpShaderIndex, unsigned int aSlot);
		void						changeWarpFboIndex(unsigned int aWarpIndex, unsigned int aWarpFboIndex, unsigned int aSlot); //aSlot 0 = A, 1 = B,...
		void                        changeFragmentShader(const string& aFragmentShaderText);
		// received shaders
		bool						hasReceivedShader() { return shaderReceived; };
		string						getReceivedShader();
		bool						hasReceivedUniforms() { return shaderUniforms; };
		string						getReceivedUniforms();
		// received stream
		string *					getBase64Image();
		bool						hasReceivedStream() { return streamReceived; };
	private:
		// Settings
		VDSettingsRef				mVDSettings;
		// Animation
		VDAnimationRef				mVDAnimation;

		void						parseMessage(string msg);
		// socketio client
		bool						clientConnected;
		int							receivedType;
		int							receivedWarpIndex;
		int							receivedFboIndex;
		int							receivedShaderIndex;
		int							receivedSlot;

		sio::client					mClient;
		double						mPingTime;

		// received shaders
		bool						shaderReceived; // TODO remove
		string						receivedFragString; // TODO remove
		bool						shaderUniforms;
		string						receivedUniformsString;
		
		string						mBase64String;
		// received stream
		bool						streamReceived;
	};
}

