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
#include "VDOscObserver.h"
#include "VDSocketIOObserver.h"
#include "VDUIObserver.h"
// VDRouterBuilder
//#include "VDRouterBuilder.h"

using namespace ci;
using namespace ci::app;

namespace videodromm
{
	typedef std::shared_ptr<class VDSession> VDSessionRef;
	typedef std::shared_ptr<class VDSessionFacade> VDSessionFacadeRef;

	class VDSessionFacade : public std::enable_shared_from_this<VDSessionFacade> {
	public:
		static VDSessionFacadeRef createVDSession(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);

		VDSessionFacadeRef		setUniformValue(unsigned int aCtrl, float aValue);
		VDSessionFacadeRef		addUIObserver(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		VDSessionFacadeRef		setupOSCReceiver();
		VDSessionFacadeRef		setupKeyboard();
		VDSessionFacadeRef		addOSCObserver(std::string host, unsigned int port);
		VDSessionFacadeRef		addSocketIOObserver(std::string host, unsigned int port);
		VDSessionFacadeRef		setAnim(unsigned int aCtrl, unsigned int aAnim);
		VDSessionFacadeRef		toggleValue(unsigned int aCtrl);
		VDSessionFacadeRef		tapTempo();
		VDSessionFacadeRef		toggleUseTimeWithTempo();
		VDSessionFacadeRef		useTimeWithTempo();
		VDSessionFacadeRef		loadFromJsonFile(const fs::path& jsonFile);
		VDSessionFacadeRef		setMode(int aMode);
		VDSessionFacadeRef		update();
		// begin terminal operations
		bool					getUseTimeWithTempo();
		bool					isOscSenderConnected();
		bool					isOscReceiverConnected();
		int						getOSCReceiverPort();
		void					setOSCReceiverPort(int aReceiverPort);
		void					setOSCMsg(const std::string& aMsg);
		std::string				getOSCMsg();
		ci::gl::TextureRef		buildRenderedMixetteTexture(unsigned int aIndex);
		ci::gl::TextureRef		buildFboTexture(unsigned int aIndex);
		ci::gl::TextureRef		buildFboRenderedTexture(unsigned int aFboIndex);
		ci::gl::TextureRef		buildPostFboTexture();
		ci::gl::TextureRef		buildWarpFboTexture();
		ci::gl::TextureRef		buildRenderedWarpFboTexture();
		unsigned int			getWarpAFboIndex(unsigned int aWarpIndex);
		unsigned int			getWarpBFboIndex(unsigned int aWarpIndex);
		float					getMinUniformValue(unsigned int aIndex);
		float					getMaxUniformValue(unsigned int aIndex);
		int						getFboTextureWidth(unsigned int aFboIndex);
		int						getFboTextureHeight(unsigned int aFboIndex);
		unsigned int			getWarpCount();
		std::string				getWarpName(unsigned int aWarpIndex);// or trycatch
		int						getWarpWidth(unsigned int aWarpIndex);
		int						getWarpHeight(unsigned int aWarpIndex);
		unsigned int			getFboListSize();
		std::string				getFboInputTextureName(unsigned int aFboIndex = 0);
		ci::gl::Texture2dRef	getFboInputTexture(unsigned int aFboIndex = 0);
		std::string				getFboName(unsigned int aFboIndex);
		int						getFFTWindowSize();
		float*					getFreqs();
		std::vector<ci::gl::GlslProg::Uniform> getUniforms(unsigned int aFboIndex = 0);
		ci::gl::Texture2dRef	buildFboInputTexture(unsigned int aFboIndex = 0);
		int						getMode();
		std::string				getModeName(unsigned int aMode);
		int						getUniformIndexForName(const std::string& aName);
		float					getUniformValue(unsigned int aCtrl);
		std::string				getUniformName(unsigned int aIndex);
		void					setIntUniformValueByIndex(unsigned int aCtrl, int aValue);
		void					setBoolUniformValueByIndex(unsigned int aCtrl, float aValue);
		// end terminal operations 
		// begin events
		bool					handleMouseMove(MouseEvent event);
		bool					handleMouseDown(MouseEvent event);
		bool					handleMouseDrag(MouseEvent event);
		bool					handleMouseUp(MouseEvent event);
		bool					handleKeyDown(KeyEvent& event);
		bool					handleKeyUp(KeyEvent& event);

		// end events
		VDSessionRef			getInstance() const;


	private:
		VDSessionFacade(VDSessionRef session, VDMediatorObservableRef mediator) : mVDSession(session), mVDMediator(mediator), mOscSenderConnected(false), mOscReceiverConnected(false) { }
		VDSessionRef						mVDSession;
		VDMediatorObservableRef				mVDMediator;
		bool								mOscSenderConnected = false;
		bool								mOscReceiverConnected = false;


	
	};


}
/*
// Builder
//VDRouterBuilderRef mVDRouterBuilder;
mVDRouterBuilder = VDRouterBuilder::createVDRouter(aVDSettings, aVDAnimation)->setWarpBFboIndex(0, 1);
mVDMediator->setUniformValue(a, b);
mVDMediator->update([](observer, { "a": 1, "b" : 2 }) -> {
	observer->setUniformValue(a, b);
});
*/