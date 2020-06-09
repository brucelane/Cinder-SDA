#include "VDMediator.h"

using namespace videodromm;

VDMediatorObservableRef VDMediatorObservable::createVDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
{
	return VDMediatorObservableRef(new VDMediatorObservable(aVDSettings, aVDAnimation));
}

VDMediatorObservable::VDMediatorObservable(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {
	CI_LOG_V("VDMediatorObservable constructor");
	mVDSettings = aVDSettings;
	mVDAnimation = aVDAnimation;
	mOSCReceiverPort = 10001;
}
VDMediatorObservableRef VDMediatorObservable::addObserver(VDUniformObserverRef o) {
	mObservers.push_back(o);
	return shared_from_this();
}
VDMediatorObservableRef VDMediatorObservable::setupOSCReceiver() {
	// Osc Receiver
	mVDOscReceiver = VDOscReceiver::create(mVDSettings, mVDAnimation);
	mOSCReceiverPort = 10001;
	fs::path jsonFile = getAssetPath("") / mOSCJsonFileName;
	loadOSCReceiverFromJsonFile(jsonFile);
	mVDOscReceiver->setupOSCReceiver(shared_from_this(), mOSCReceiverPort);
	saveOSCReceiverToJson();
	return shared_from_this();
}
void VDMediatorObservable::loadOSCReceiverFromJsonFile(const fs::path& jsonFile) {
	if (fs::exists(jsonFile)) {
		JsonTree json(loadFile(jsonFile));
		if (json.hasChild("uniform")) {
			JsonTree u(json.getChild("uniform"));
			mOSCReceiverPort = (u.hasChild("index")) ? u.getValueForKey<int>("index") : 251;
		}		
	}
}
//! to json
JsonTree VDMediatorObservable::saveOSCReceiverToJson() const
{
	JsonTree json;
	JsonTree receiver = ci::JsonTree::makeArray("receiver");
	receiver.addChild(ci::JsonTree("receiverport", mOSCReceiverPort));
	json.addChild(receiver);
	fs::path jsonFile = getAssetPath("") / mOSCJsonFileName;
	json.write(jsonFile);
	return json;
}

int VDMediatorObservable::getOSCReceiverPort() {
	return mOSCReceiverPort;
};
void VDMediatorObservable::setOSCReceiverPort(int aReceiverPort) {
	mOSCReceiverPort = aReceiverPort;
};
void VDMediatorObservable::setOSCMsg(const std::string& aMsg) {
	mVDOscReceiver->setOSCMsg(aMsg);
};
std::string VDMediatorObservable::getOSCMsg() {
	return mVDOscReceiver->getOSCMsg();
}
VDMediatorObservableRef VDMediatorObservable::setupKeyboard() {
	// Keyboard
	mVDKeyboard = VDKeyboard::create(mVDSettings, mVDAnimation);
	mVDKeyboard->setupKeyboard(shared_from_this());
	return shared_from_this();
}
bool VDMediatorObservable::handleKeyDown(KeyEvent& event) {
	return mVDKeyboard->handleKeyDown(event);
}
bool VDMediatorObservable::handleKeyUp(KeyEvent& event) {
	return mVDKeyboard->handleKeyUp(event);
}
float VDMediatorObservable::getUniformValue(unsigned int aIndex) {
	return mVDAnimation->getUniformValue(aIndex);
}
std::string VDMediatorObservable::getUniformName(unsigned int aIndex) {
	return mVDAnimation->getUniformName(aIndex);
}
VDMediatorObservableRef VDMediatorObservable::setUniformValue(int aIndex, float aValue) {
	if (aIndex != mVDSettings->IFPS) {
		for (auto observer : mObservers) {
			observer->setUniformValue(aIndex, aValue);
		}
	}
	return shared_from_this();
};

VDMediatorObservableRef VDMediatorObservable::updateShaderText(int aIndex, float aValue) {
	for (auto observer : mObservers) {
		observer->setUniformValue(aIndex, aValue);
	}
	return shared_from_this();
};