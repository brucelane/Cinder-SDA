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
			//VDSessionRef Session = VDSessionFacade::createVDSession(aVDSettings, aVDAnimation)->setWarpBFboIndex(1, 1)->setWarpAFboIndex(1, 2)->getInstance();
			VDMediatorObservableRef mVDMediator =
				VDMediatorObservable::createVDMediatorObservable(aVDSettings, aVDAnimation);
				//->addObserver(VDSocketIOObserverBuilder->connect(ip, port))->addObserver(new UIDisplay());

			//mVDMediator->updateUniformValue(a, b);
			/*mVDMediator->update([](observer, { "a": 1, "b" : 2 }) -> {
				observer->setUniformValue(a, b);
			});*/
			//return VDRouterBuilderRef(new VDRouterBuilder(VDRouterRef(new VDRouter(aVDSettings, aVDAnimation))));
			return VDSessionFacadeRef(new VDSessionFacade(VDSessionRef(new VDSession(aVDSettings, aVDAnimation))));
		}
		VDSessionFacadeRef loadFromJsonFile(const fs::path& jsonFile) {
			mVDSession->loadFromJsonFile(jsonFile);
			return shared_from_this();
		}
		VDSessionFacadeRef update() {
			mVDSession->update();
			return shared_from_this();
		}
		// operation terminale
		ci::gl::TextureRef buildRenderedMixetteTexture(unsigned int aIndex) {
			return mVDSession->getRenderedMixetteTexture(aIndex);
		}
		// operation terminale
		ci::gl::TextureRef buildFboTexture(unsigned int aIndex) {
			return mVDSession->getFboTexture(aIndex);;
		}
		VDSessionRef getInstance() const {
			return mVDSession;
		}

	private:
		VDSessionFacade(VDSessionRef session) : mVDSession(session) { }
		VDSessionRef mVDSession;
	};


}