#include "VDRouter.h"

using namespace videodromm;

VDRouter::VDRouter(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	CI_LOG_V("VDRouter constructor");
	mFBOAChanged = false;
	mFBOBChanged = false;
	// midi
	if (mVDSettings->mMIDIOpenAllInputPorts) midiSetup();

	mSelectedWarp = 0;
	mSelectedFboA = 1;
	mSelectedFboB = 2;
}

void VDRouter::shutdown() {

	mMidiIn0.closePort();
	mMidiIn1.closePort();
	mMidiIn2.closePort();
	mMidiOut0.closePort();
	mMidiOut1.closePort();
	mMidiOut2.closePort();

}
void VDRouter::saveMidiPorts() {
	JsonTree		json;
	JsonTree midiin = ci::JsonTree::makeArray("midiin");
	for (int j = 0; j < mMidiInputs.size(); j++)
	{
		midiin.addChild(ci::JsonTree("midiinname", mMidiInputs[j].portName));
	}
	json.addChild(midiin);
	JsonTree midiout = ci::JsonTree::makeArray("midiout");
	for (int j = 0; j < mMidiOutputs.size(); j++)
	{
		midiout.addChild(ci::JsonTree("midioutname", mMidiOutputs[j].portName));
	}
	json.addChild(midiout);
	string jsonFileName = "midi.json";
	fs::path jsonFile = getAssetPath("") / jsonFileName;
	json.write(jsonFile);
}
void VDRouter::midiSetup() {
	stringstream ss;
	ss << "setupMidi ";
	CI_LOG_V("midiSetup: " + ss.str());
	if (mMidiIn0.getNumPorts() > 0)
	{
		mMidiIn0.listPorts();
		for (int i = 0; i < mMidiIn0.getNumPorts(); i++)
		{
			bool alreadyListed = false;
			for (int j = 0; j < mMidiInputs.size(); j++)
			{
				if (mMidiInputs[j].portName == mMidiIn0.getPortName(i)) alreadyListed = true;
			}
			if (!alreadyListed) {
				midiInput mIn;
				mIn.portName = mMidiIn0.getPortName(i);
				mMidiInputs.push_back(mIn);
				if (mVDSettings->mMIDIOpenAllInputPorts) {
					openMidiInPort(i);
					mMidiInputs[i].isConnected = true;
					ss << "Opening MIDI in port " << i << " " << mMidiInputs[i].portName;
				}
				else {
					mMidiInputs[i].isConnected = false;
					ss << "Available MIDI in port " << i << " " << mMidiIn0.getPortName(i);
				}
			}
		}
	}
	else {
		ss << "no midi in ports found";
	}

	// midi out
	//mMidiOut0.getPortList();
	if (mMidiOut0.getNumPorts() > 0) {
		for (int i = 0; i < mMidiOut0.getNumPorts(); i++)
		{
			bool alreadyListed = false;
			for (int j = 0; j < mMidiOutputs.size(); j++)
			{
				if (mMidiOutputs[j].portName == mMidiOut0.getPortName(i)) alreadyListed = true;
			}
			if (!alreadyListed) {
				midiOutput mOut;
				mOut.portName = mMidiOut0.getPortName(i);
				mMidiOutputs.push_back(mOut);

				mMidiOutputs[i].isConnected = false;
				ss << "Available MIDI output port " << i << " " << mMidiOut0.getPortName(i);

			}
		}
	}
	else {
		ss << "no midi out Ports found";
	}
	saveMidiPorts();
	midiControlType = "none";
	midiControl = midiPitch = midiVelocity = midiNormalizedValue = midiValue = midiChannel = 0;
	ss << std::endl;
	mVDSettings->mMidiMsg = ss.str() + "\n" + mVDSettings->mMidiMsg.substr(0, mVDSettings->mMsgLength);
	CI_LOG_V(ss.str());
}

void VDRouter::openMidiInPort(int i) {
	CI_LOG_V("openMidiInPort: " + toString(i));
	stringstream ss;
	if (i < mMidiIn0.getNumPorts()) {
		if (i == 0) {
			mMidiIn0.openPort(i);
			mMidiIn0.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
		}
		if (i == 1) {
			mMidiIn1.openPort(i);
			mMidiIn1.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
		}
		if (i == 2) {
			mMidiIn2.openPort(i);
			mMidiIn2.midiSignal.connect(std::bind(&VDRouter::midiListener, this, std::placeholders::_1));
		}
	}
	mMidiInputs[i].isConnected = true;
	ss << "Opening MIDI in port " << i << " " << mMidiInputs[i].portName << std::endl;

	mVDSettings->mMidiMsg = ss.str() + "\n" + mVDSettings->mMidiMsg.substr(0, mVDSettings->mMsgLength);

	CI_LOG_V(ss.str());
}
void VDRouter::closeMidiInPort(int i) {

	if (i == 0)
	{
		mMidiIn0.closePort();
	}
	if (i == 1)
	{
		mMidiIn1.closePort();
	}
	if (i == 2)
	{
		mMidiIn2.closePort();
	}
	mMidiInputs[i].isConnected = false;

}
void VDRouter::midiOutSendNoteOn(int i, int channel, int pitch, int velocity) {

	if (i == 0)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut0.sendNoteOn(channel, pitch, velocity);
	}
	if (i == 1)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut1.sendNoteOn(channel, pitch, velocity);
	}
	if (i == 2)
	{
		if (mMidiOutputs[i].isConnected) mMidiOut2.sendNoteOn(channel, pitch, velocity);
	}

}
void VDRouter::openMidiOutPort(int i) {

	stringstream ss;
	ss << "Port " << i;
	if (i < mMidiOutputs.size()) {
		if (i == 0) {
			if (mMidiOut0.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << " Opened MIDI out port " << i << " " << mMidiOutputs[i].portName;
				mMidiOut0.sendNoteOn(1, 40, 64);
			}
			else {
				ss << " Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName;
			}
		}
		if (i == 1) {
			if (mMidiOut1.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << " Opened MIDI out port " << i << " " << mMidiOutputs[i].portName;
				mMidiOut1.sendNoteOn(1, 40, 64);
			}
			else {
				ss << " Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName;
			}
		}
		if (i == 2) {
			if (mMidiOut2.openPort(i)) {
				mMidiOutputs[i].isConnected = true;
				ss << " Opened MIDI out port " << i << " " << mMidiOutputs[i].portName;
				mMidiOut2.sendNoteOn(1, 40, 64);
			}
			else {
				ss << " Can't open MIDI out port " << i << " " << mMidiOutputs[i].portName;
			}
		}
	}
	ss << std::endl;
	mVDSettings->mMidiMsg = ss.str() + "\n" + mVDSettings->mMidiMsg.substr(0, mVDSettings->mMsgLength);
	CI_LOG_V(ss.str());
}
void VDRouter::closeMidiOutPort(int i) {

	if (i == 0)
	{
		mMidiOut0.closePort();
	}
	if (i == 1)
	{
		mMidiOut1.closePort();
	}
	if (i == 2)
	{
		mMidiOut2.closePort();
	}
	mMidiOutputs[i].isConnected = false;

}

void VDRouter::midiListener(midi::Message msg) {
	stringstream ss;
	ss << "MIDI port: " << mMidiIn0.getPortName(msg.port);
	midiChannel = msg.channel;
	switch (msg.status)
	{
	case MIDI_CONTROL_CHANGE:
		midiControlType = "/cc";
		midiControl = msg.control;
		midiValue = msg.value;
		midiNormalizedValue = lmap<float>(midiValue, 0.0, 127.0, 0.0, 1.0);
		ss << " cc Chn: " << midiChannel << " CC: " << midiControl << " Val: " << midiValue << " NVal: " << midiNormalizedValue;
		CI_LOG_V("Midi: " + ss.str());

		if (midiControl > 20 && midiControl < 49) {
			/*if (midiControl > 20 && midiControl < 29) {
				mSelectedWarp = midiControl - 21;
			}
			if (midiControl > 40 && midiControl < 49) {
				mSelectedFboB = midiControl - 41;
				mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, mSelectedFboB);
			}
			*/
			//if (midiControl > 30 && midiControl < 39) {
			//! 20200526 mVDSocketio->changeFloatValue(midiControl, midiNormalizedValue);
			//mSelectedFboA = midiControl - 31;
			//mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, mSelectedFboA);
		//}

		}
		else {
			updateParams(midiControl, midiNormalizedValue);
		}
		//mSocketIO->write("{\"params\" :[{" + controlType);
		break;
	case MIDI_NOTE_ON:
		/*
		TODO nano notes instad of cc
		if (midiControl > 20 && midiControl < 29) {
				mSelectedWarp = midiControl - 21;
			}
			if (midiControl > 30 && midiControl < 39) {
				mSelectedFboA = midiControl - 31;
				mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, mSelectedFboA);
			}
			if (midiControl > 40 && midiControl < 49) {
				mSelectedFboB = midiControl - 41;
				mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, mSelectedFboB);
			}
		*/
		midiControlType = "/on";
		//midiVelocity = msg.velocity;
		//midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);
		//// quick hack!
		//mVDAnimation->setUniformValue(14, 1.0f + midiNormalizedValue);
		midiPitch = msg.pitch;
		// midimix solo mode
		/*if (midiPitch == 27) midiSticky = true;
		if (midiSticky) {
			midiStickyPrevIndex = midiPitch;
			midiStickyPrevValue = mVDAnimation->getBoolUniformValueByIndex(midiPitch + 80);
		}*/
		//mVDAnimation->setBoolUniformValueByIndex(midiPitch + 80, true);

		// This does mVDSession->setFboFragmentShaderIndex(0, midiPitch);
		if (midiPitch < 9) {
			mSelectedFboA = midiPitch;
			mFBOAChanged = true;
		}
		if (midiPitch > 8 && midiPitch < 17) {
			mSelectedFboB = midiPitch - 8;
			mFBOBChanged = true;
		}
		if (midiPitch > 17 && midiPitch < 24) {
			mVDAnimation->setBoolUniformValueByIndex(midiPitch + 80 - 17, true);
		}
		ss << " noteon Chn: " << midiChannel << " Pitch: " << midiPitch;
		CI_LOG_V("Midi: " + ss.str());
		break;
	case MIDI_NOTE_OFF:
		midiPitch = msg.pitch;
		if (midiPitch > 17 && midiPitch < 24) {
			mVDAnimation->setBoolUniformValueByIndex(midiPitch + 80 - 17, false);
		}
		// midimix solo mode
		/*if (midiPitch == 27) {
			midiStickyPrevIndex = 0;
			midiSticky = false;
		}
		if (!midiSticky) {*/
		//mVDAnimation->setBoolUniformValueByIndex(midiPitch + 80, false);
	/*}
	else {
		if (midiPitch == midiStickyPrevIndex) {
			mVDAnimation->setBoolUniformValueByIndex(midiPitch + 80, !midiStickyPrevValue);
		}
	}*/
		ss << " noteoff Chn: " << midiChannel << " Pitch: " << midiPitch;
		CI_LOG_V("Midi: " + ss.str());
		midiControlType = "/off";
		/*midiPitch = msg.pitch;
		midiVelocity = msg.velocity;
		midiNormalizedValue = lmap<float>(midiVelocity, 0.0, 127.0, 0.0, 1.0);*/
		break;
	case MIDI_PITCH_BEND:
		midiControlType = "/pb";
		midiControl = msg.control;
		midiValue = msg.value;
		midiNormalizedValue = lmap<float>(midiValue, 0.0, 127.0, 0.0, 1.0);
		ss << " pb Chn: " << midiChannel << " CC: " << midiControl << " Val: " << midiValue << " NVal: " << midiNormalizedValue;
		mVDAnimation->setUniformValue(mVDSettings->IMOUSEX, midiValue);
		break;
	default:
		ss << " unknown status: " << msg.status;
		CI_LOG_V("Midi: " + ss.str());
		break;
	}
	//ss << "MIDI Chn: " << midiChannel << " type: " << midiControlType << " CC: " << midiControl << " Pitch: " << midiPitch << " Vel: " << midiVelocity << " Val: " << midiValue << " NVal: " << midiNormalizedValue << std::endl;
	//CI_LOG_V("Midi: " + ss.str());
	ss << std::endl;
	mVDSettings->mMidiMsg = ss.str() + "\n" + mVDSettings->mMidiMsg.substr(0, mVDSettings->mMsgLength);
}

void VDRouter::updateParams(int iarg0, float farg1) {
	if (farg1 > 0.1) {
		//avoid to send twice
		if (iarg0 == 58) {
			// track left		
			mVDSettings->iTrack--;
			if (mVDSettings->iTrack < 0) mVDSettings->iTrack = 0;
		}
		if (iarg0 == 59) {
			// track right
			mVDSettings->iTrack++;
			if (mVDSettings->iTrack > 7) mVDSettings->iTrack = 7;
		}
		if (iarg0 == 60) {
			// set (reset blendmode)
			mVDSettings->iBlendmode = 0;
		}
		if (iarg0 == 61) {
			// right arrow
			mVDSettings->iBlendmode--;
			if (mVDSettings->iBlendmode < 0) mVDSettings->iBlendmode = mVDAnimation->getBlendModesCount() - 1;
		}
		if (iarg0 == 62) {
			// left arrow
			mVDSettings->iBlendmode++;
			if (mVDSettings->iBlendmode > mVDAnimation->getBlendModesCount() - 1) mVDSettings->iBlendmode = 0;
		}
	}
	if (iarg0 > 0 && iarg0 < 9) {
		// sliders 
		//! 20200526 mVDSocketio->changeFloatValue(iarg0, farg1);
		
		mVDAnimation->setUniformValue(iarg0, farg1);
	}
	if (iarg0 > 10 && iarg0 < 19) {
		// rotary 
		//! 20200526 
		//mVDSocketio->changeFloatValue(iarg0, farg1);
		// audio multfactor
		//! 20200526 if (iarg0 == 13) mVDSocketio->changeFloatValue(iarg0, (farg1 + 0.01) * 10);
		// exposure
		//! 20200526 if (iarg0 == 14) mVDSocketio->changeFloatValue(iarg0, (farg1 + 0.01) * mVDAnimation->getMaxUniformValueByIndex(14));
		// xfade
		if (iarg0 == mVDSettings->IXFADE) {//18
			//! 20200526 mVDSocketio->changeFloatValue(iarg0, farg1);
			//mVDSettings->xFade = farg1;
			//mVDSettings->xFadeChanged = true;
		}
	}
	// buttons
	if (iarg0 > 20 && iarg0 < 29) {
		// top row
		//! 20200526 mVDSocketio->changeFloatValue(iarg0, farg1);
	}
	if (iarg0 > 30 && iarg0 < 39)
	{
		// middle row
		//! 20200526 mVDSocketio->changeFloatValue(iarg0, farg1);
		//mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOA, iarg0 - 31);
	}
	if (iarg0 > 40 && iarg0 < 49) {
		// low row 
		//! 20200526 mVDSocketio->changeFloatValue(iarg0, farg1);
		//mVDAnimation->setIntUniformValueByIndex(mVDSettings->IFBOB, iarg0 - 41);
	}
	//if (iarg0 > 0 && iarg0 < 49) {
		// float values 
		//mVDSocketio->wsWrite("{\"params\" :[{ \"name\":" + toString(iarg0) + ",\"value\":" + toString(mVDAnimation->getUniformValue(iarg0)) + "}]}");
	//}
}


void VDRouter::colorWrite()
{
#if defined( CINDER_MSW )
	// lights4events
	char col[97];
	int r = (int)(mVDAnimation->getUniformValue(1) * 255);
	int g = (int)(mVDAnimation->getUniformValue(2) * 255);
	int b = (int)(mVDAnimation->getUniformValue(3) * 255);
	int a = (int)(mVDAnimation->getUniformValue(4) * 255);
	//sprintf(col, "#%02X%02X%02X", r, g, b);
	sprintf(col, "{\"type\":\"action\", \"parameters\":{\"name\":\"FC\",\"parameters\":{\"color\":\"#%02X%02X%02X%02X\",\"fading\":\"NONE\"}}}", a, r, g, b);
	//! 20200526 mVDSocketio->wsWrite(col);
#endif
}
