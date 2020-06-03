/*
	VDSessionFacade

*/
// TODO 

#pragma once
#include "cinder/Cinder.h"
#include "cinder/app/App.h"
// Logger
#include "VDLog.h"
// Settings
#include "VDSettings.h"
//
#include "VDSession.h"
#include "VDAnimation.h"
#include "VDMediator.h"
#include "VDOscSender.h"
// VDRouterBuilder
#include "VDRouterBuilder.h"

using namespace ci;
using namespace ci::app;
//using namespace std;
namespace videodromm
{
	typedef std::shared_ptr<class VDSession> VDSessionRef;
	typedef std::shared_ptr<class VDSessionFacade> VDSessionFacadeRef;

	class VDSessionFacade : public std::enable_shared_from_this<VDSessionFacade> {
	public:
		static VDSessionFacadeRef createVDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation)
		{
			
			VDMediatorObservableRef mediator =
				VDMediatorObservable::createVDMediatorObservable(aVDSettings, aVDAnimation)
				->addObserver(VDSocketIOObserver::connect(aVDSettings->mSocketIOHost, aVDSettings->mSocketIOPort))
				->addObserver(VDOscObserver::connect(aVDSettings->mOSCDestinationHost, aVDSettings->mOSCDestinationPort));
				//->addObserver(new UIDisplay());		
			return VDSessionFacadeRef(new VDSessionFacade(VDSessionRef(new VDSession(aVDSettings, aVDAnimation)), mediator));
		}
		
		//mVDRouterBuilder = VDRouterBuilder::createVDRouter(aVDSettings, aVDAnimation)->setWarpBFboIndex(0, 1);
		void							setIntUniformValueByIndex(unsigned int aCtrl, int aValue) {
			//mVDRouterBuilder->changeIntValue(aCtrl, aValue);
			mVDMediator->updateUniformValue(aCtrl, aValue);
		}
		void							setBoolUniformValueByIndex(unsigned int aCtrl, float aValue) {
			// done in router mVDAnimation->changeFloatValue(aCtrl, aValue);
			//mVDRouterBuilder->changeBoolValue(aCtrl, aValue);
			mVDMediator->updateUniformValue(aCtrl, aValue);
		}
		// OSC
		void							setupOSCSender() {
			//mVDMediator->addObserver(VDOscObserver::connect(mVDSettings->mOSCDestinationHost, mVDSettings->mOSCDestinationPort));
			//mVDRouterBuilder->setupOSCSender();
			//mediator->addObserver(VDOscObserver::connect(aVDSettings->mSocketIOHost, aVDSettings->mSocketIOPort));
		}	
		VDSessionFacadeRef setupOSCReceiver() {
			// done in router mVDAnimation->changeFloatValue(aCtrl, aValue);
			//mVDRouterBuilder->setupOSCReceiver();
			return shared_from_this();
		}
		VDSessionFacadeRef setUniformValue(unsigned int aCtrl, float aValue) {
			mVDMediator->updateUniformValue(aCtrl, aValue);
			//mVDRouterBuilder->changeFloatValue(aCtrl, aValue);
			return shared_from_this();
		}

		VDSessionFacadeRef toggleValue(unsigned int aCtrl) {
			mVDSession->toggleValue(aCtrl);
			return shared_from_this();
		}

		VDSessionFacadeRef  setBpm(float aBpm) {
			mVDSession->setBpm(aBpm);
			return shared_from_this();
		};
		VDSessionFacadeRef  tapTempo() {
			mVDSession->tapTempo();
			return shared_from_this();
		};
		VDSessionFacadeRef  toggleUseTimeWithTempo() {
			mVDSession->toggleUseTimeWithTempo();
			return shared_from_this();
		};
		VDSessionFacadeRef  useTimeWithTempo() {
			mVDSession->useTimeWithTempo();
			return shared_from_this();
		};
		VDSessionFacadeRef loadFromJsonFile(const fs::path& jsonFile) {
			mVDSession->loadFromJsonFile(jsonFile);
			return shared_from_this();
		}
		VDSessionFacadeRef setMode(int aMode) {
			mVDSession->setMode(aMode);
			return shared_from_this();
		}
		VDSessionFacadeRef update() {
			mVDSession->update();
			return shared_from_this();
		}
		// begin terminal operations
		bool getUseTimeWithTempo() {
			return mVDSession->getUseTimeWithTempo();
		};
		ci::gl::TextureRef buildRenderedMixetteTexture(unsigned int aIndex) {
			return mVDSession->getRenderedMixetteTexture(aIndex);
		}
		ci::gl::TextureRef buildFboTexture(unsigned int aIndex) {
			return mVDSession->getFboTexture(aIndex);;
		}
		ci::gl::TextureRef buildFboRenderedTexture(unsigned int aFboIndex) {
			return mVDSession->getFboRenderedTexture(aFboIndex);
		}
		ci::gl::TextureRef buildPostFboTexture() {
			return mVDSession->getPostFboTexture();
		}
		ci::gl::TextureRef buildWarpFboTexture() {
			return mVDSession->getWarpFboTexture();
		}
		ci::gl::TextureRef buildRenderedWarpFboTexture() {
			return mVDSession->getRenderedWarpFboTexture();
		}
		unsigned int getWarpAFboIndex(unsigned int aWarpIndex) {
			return mVDSession->getWarpAFboIndex(aWarpIndex);
		}
		unsigned int getWarpBFboIndex(unsigned int aWarpIndex) {
			return mVDSession->getWarpBFboIndex(aWarpIndex);
		}

		float getMaxVolume() {
			return mVDSession->getMaxVolume();
		};
		float getBpm() {
			return mVDSession->getBpm();
		};
		float getUniformValue(unsigned int aIndex) {
			return mVDSession->getMinUniformValue(aIndex);
		}
		float getMinUniformValue(unsigned int aIndex) {
			return mVDSession->getMinUniformValue(aIndex);
		}
		float getMaxUniformValue(unsigned int aIndex) {
			return mVDSession->getMinUniformValue(aIndex);
		}
		int getFboTextureWidth(unsigned int aFboIndex) {
			return mVDSession->getFboTextureWidth(aFboIndex);
		};
		int getFboTextureHeight(unsigned int aFboIndex) {
			return mVDSession->getFboTextureHeight(aFboIndex);
		}
		unsigned int getWarpCount() {
			return mVDSession->getWarpCount();
		};
		std::string getWarpName(unsigned int aWarpIndex) {
			return mVDSession->getWarpName(aWarpIndex);
		}// or trycatch
		int getWarpWidth(unsigned int aWarpIndex) {
			return mVDSession->getWarpWidth(aWarpIndex);
		}
		int getWarpHeight(unsigned int aWarpIndex) {
			return mVDSession->getWarpHeight(aWarpIndex);
		}
		unsigned int getFboListSize() {
			return mVDSession->getFboListSize();
		}
		std::string getFboInputTextureName(unsigned int aFboIndex = 0) {
			return mVDSession->getFboInputTextureName(aFboIndex);
		}
		ci::gl::Texture2dRef getFboInputTexture(unsigned int aFboIndex = 0) {
			return mVDSession->getFboInputTexture(aFboIndex);
		}
		std::string getFboName(unsigned int aFboIndex) {
			return mVDSession->getFboName(aFboIndex);
		}
		int getFFTWindowSize() {
			return mVDSession->getFFTWindowSize();
		}
		float* getFreqs() {
			return mVDSession->getFreqs();
		}
		std::vector<ci::gl::GlslProg::Uniform> getUniforms(unsigned int aFboIndex = 0) {
			return mVDSession->getUniforms(aFboIndex);
		}
		ci::gl::Texture2dRef buildFboInputTexture(unsigned int aFboIndex = 0) {
			return mVDSession->getFboInputTexture(aFboIndex);
		}
		int getMode() {
			return mVDSession->getMode();
		}
		std::string getModeName(unsigned int aMode) {
			return mVDSession->getModeName(aMode);
		}
		int getUniformIndexForName(const std::string& aName) {
			return mVDSession->getUniformIndexForName(aName);
		};
		// end terminal operations 
		VDSessionRef getInstance() const {
			return mVDSession;
		}

	private:
		VDSessionFacade(VDSessionRef session, VDMediatorObservableRef mediator) : mVDSession(session), mVDMediator(mediator) { }
		VDSessionRef mVDSession;
		//VDSettingsRef mVDSettings;
		//VDAnimationRef mVDAnimation;
		VDMediatorObservableRef mVDMediator;
		// Builder
		//VDRouterBuilderRef mVDRouterBuilder;
	};


}

//mVDMediator->updateUniformValue(a, b);
		/*mVDMediator->update([](observer, { "a": 1, "b" : 2 }) -> {
			observer->setUniformValue(a, b);
		});*/