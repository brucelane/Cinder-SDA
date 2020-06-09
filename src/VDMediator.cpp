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
}
VDMediatorObservableRef VDMediatorObservable::addObserver(VDUniformObserverRef o) {
	mObservers.push_back(o);
	return shared_from_this();
}
VDMediatorObservableRef VDMediatorObservable::setupOSCReceiver() {
	// Osc Receiver
	mVDOscReceiver = VDOscReceiver::create(mVDSettings, mVDAnimation);
	mVDOscReceiver->setupOSCReceiver(shared_from_this());
	return shared_from_this();
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