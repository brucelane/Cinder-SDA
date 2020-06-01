//
//  VDsession.cpp
//

#include "VDSession.h"

using namespace videodromm;

VDSession::VDSession(VDSettingsRef aVDSettings)
{
	CI_LOG_V("VDSession ctor");
	mVDSettings = aVDSettings;
	// Utils
	mVDUtils = VDUtils::create(mVDSettings);
	// Animation
	mVDAnimation = VDAnimation::create(mVDSettings);
	// TODO: needed? mVDAnimation->tapTempo();
	// Mix
	mVDMix = VDMix::create(mVDSettings, mVDAnimation);
	//createFboShaderTexture("default.fs", "0.jpg");
	//createFboShaderTexture("audio.fs", "audio");
	// allow log to file
	mVDLog = VDLog::create();
	// fbo
	gl::Fbo::Format format;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing

	fboFmt.setColorTextureFormat(fmt);
	mWarpsFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, format.depthTexture());
	mPostFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, format.depthTexture());
	mGlslPost = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("passthrough.vs")).fragment(loadAsset("post.glsl")));
	mWarpTexture = ci::gl::Texture::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, ci::gl::Texture::Format().loadTopDown());
	// adjust the content size of the warps

	// TODO 20200305 was 20200302 if (getFboRenderedTexture(0)) Warp::setSize(mWarpList, getFboRenderedTexture(0)->getSize());
	Warp::setSize(mWarpList, ivec2(mVDSettings->mFboWidth, mVDSettings->mFboHeight)); //
	// initialize warps
	mSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
	if (fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarpList = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
		mWarpList.push_back(WarpPerspectiveBilinear::create());
	}
	loadFbos();
	//loadWarps();
	// init fbo format
	//fmt.setWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	//fmt.setBorderColor(Color::black());
	// uncomment this to enable 4x antialiasing
	//fboFmt.setSamples( 4 );
	/*fboFmt.setColorTextureFormat(fmt);
	mPosX = mPosY = 0.0f;
	mZoom = 1.0f;
	mSelectedWarp = 0;*/
	// Modes
	mModesList[0] = "Mixette";
	mModesList[1] = "Fbo0";
	mModesList[2] = "Fbo1";
	mModesList[3] = "Fbo2";
	mModesList[4] = "Fbo3";
	mModesList[5] = "Fbo4";
	mModesList[6] = "Fbo5";
	mModesList[7] = "Fbo6";
	mModesList[8] = "Fbo7";
	mModesList[9] = "Fbo8";
	mMode = 0;
	
	// Message router
	//mVDRouter = VDRouterBuilder::createVDRouter(mVDSettings, mVDAnimation)->addShader(0,6)->getInstance();
	mVDBuilder = VDRouterBuilder::createVDRouter(mVDSettings, mVDAnimation)->setWarpBFboIndex(0, 1);	
	//Attente d'une récupération de données via socket io
	mVDBuilder->setWarpAFboIndex(1, 1)->setWarpBFboIndex(1,0);
	//Attente d'une autre autre récupération de données via socket io
	mVDBuilder->setWarpAFboIndex(0, 0);

	//mVDRouter = builder->getInstance();

	// reset no matter what, so we don't miss anything
	cmd = -1;
	mFreqWSSend = false;
	reset();

	mCurrentBlend = 0;
	// check to see if VDSession.xml file exists and restore if it does
	sessionPath = getAssetPath("") / mVDSettings->mAssetsPath / sessionFileName;
	if (fs::exists(sessionPath))
	{
		restore();
	}
	else
	{
		// Create json file if it doesn't already exist.
		std::ofstream oStream(sessionPath.string());
		oStream.close();
		save();
	}
}
void VDSession::loadFromJsonFile(const fs::path& jsonFile) {
	if (fs::exists(jsonFile)) {
		JsonTree json(loadFile(jsonFile));
		fboFromJson(json);
	}
}
void VDSession::loadFbos() {

	int f = 0;
	bool found = true;
	std::string shaderFileName;
	std::string textureFileName;
	while (found) {
		std::string jsonFileName = "fbo" + toString(f) + ".json";
	
		fs::path jsonFile = getAssetPath("") / mVDSettings->mAssetsPath / jsonFileName;
		if (fs::exists(jsonFile)) {
// nouveau
	/*loadFromJsonFile(jsonFile)
			->createShader()
			->createUniforms()
			->compile()
			->createFboWhenSuccess()
			->addToFboList();*/
	// ancien
			JsonTree json(loadFile(jsonFile));
			fboFromJson(json);
			f++;
		}
		else {
			found = false;
		}
	} //while

}

void VDSession::update(unsigned int aClassIndex) {
	
	// fps calculated in main app
	mVDSettings->sFps = toString(floor(getUniformValue(mVDSettings->IFPS)));
	mVDAnimation->update();
	
	mVDMix->getMixetteTexture(0);
	renderWarpsToFbo();
	renderPostToFbo();
}
void VDSession::renderPostToFbo()
{
	{
		gl::ScopedFramebuffer fbScp(mPostFbo);
		// clear out the FBO with black
		gl::clear(Color::black());
		//gl::clear(ColorA(0.4f, 0.8f, 0.0f, 0.3f));

		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mPostFbo->getSize());

		// texture binding must be before ScopedGlslProg
		//mWarpsFbo->getColorTexture()
		mWarpTexture->bind(40);
		gl::ScopedGlslProg prog(mGlslPost);

		// not used yet mGlslPost->uniform("TIME", getUniformValue(mVDSettings->ITIME) - mVDSettings->iStart);;
		mGlslPost->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
		mGlslPost->uniform("iChannel0", 40); // texture 0
		mGlslPost->uniform("iSobel", getUniformValue(mVDSettings->ISOBEL));
		mGlslPost->uniform("iExposure", getUniformValue(mVDSettings->IEXPOSURE));
		mGlslPost->uniform("iTrixels", getUniformValue(mVDSettings->ITRIXELS)); // trixels if > 0.
		mGlslPost->uniform("iZoom", getUniformValue(mVDSettings->IZOOM));
		mGlslPost->uniform("iChromatic", getUniformValue(mVDSettings->ICHROMATIC));
		mGlslPost->uniform("iFlipV", (int)getBoolUniformValueByIndex(mVDSettings->IFLIPPOSTV));
		mGlslPost->uniform("iFlipH", (int)getBoolUniformValueByIndex(mVDSettings->IFLIPPOSTH));
		mGlslPost->uniform("iInvert", (int)getBoolUniformValueByIndex(mVDSettings->IINVERT));
		gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
	}
}
void VDSession::renderWarpsToFbo()
{
	{
		gl::ScopedFramebuffer fbScp(mWarpsFbo);
		// clear out the FBO with black
		//gl::clear(Color::black());
		gl::clear(ColorA(0.4f, 0.0f, 0.8f, 0.3f));

		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mWarpsFbo->getSize());
		// iterate over the warps and draw their content
		int i = 0;
		int a = 0;
		int s = 0;
		for (auto &warp : mWarpList) {
			a = warp->getAFboIndex();
			if (a < 0) a = 0; // TODO 20200228 a could be negative if warps3.xml > warps01.json
			i = math<int>::min(a, getFboListSize() - 1);
			s = getFboListSize(); // TMP
			//if (isFboValid(i)) {
				//ko  warp->draw(getFboRenderedTexture(0));
				//ko warp->draw(getFboTexture(0)); bind to 0 broken
			warp->draw(mVDMix->getRenderedMixetteTexture(0));
			//}

		}
		//gl::color(0.5, 0.0, 1.0, 0.4f);
		//gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth/2, mVDSettings->mFboHeight/2));
		mWarpTexture = mWarpsFbo->getColorTexture();
	}
}
bool VDSession::save()
{
	saveFbos();
	saveWarps();
	// save uniforms settings
	//mVDAnimation->save();
	// save in sessionPath
	// TODO add shaders section
	JsonTree doc;

	JsonTree settings = JsonTree::makeArray("settings");
	settings.addChild(ci::JsonTree("bpm", mOriginalBpm));
	settings.addChild(ci::JsonTree("beatsperbar", mVDAnimation->getIntUniformValueByName("iBeatsPerBar")));
	//settings.addChild(ci::JsonTree("fadeindelay", mFadeInDelay));
	//settings.addChild(ci::JsonTree("fadeoutdelay", mFadeOutDelay));
	settings.addChild(ci::JsonTree("endframe", mVDAnimation->mEndFrame));
	doc.pushBack(settings);

	/*JsonTree assets = JsonTree::makeArray("assets");
	if (mWaveFileName.length() > 0) assets.addChild(ci::JsonTree("wavefile", mWaveFileName));
	assets.addChild(ci::JsonTree("waveplaybackdelay", mWavePlaybackDelay));
	if (mMovieFileName.length() > 0) assets.addChild(ci::JsonTree("moviefile", mMovieFileName));
	assets.addChild(ci::JsonTree("movieplaybackdelay", mMoviePlaybackDelay));
	if (mImageSequencePath.length() > 0) assets.addChild(ci::JsonTree("imagesequencepath", mImageSequencePath));
	if (mText.length() > 0) {
		assets.addChild(ci::JsonTree("text", mText));
		assets.addChild(ci::JsonTree("textplaybackdelay", mTextPlaybackDelay));
		assets.addChild(ci::JsonTree("textplaybackend", mTextPlaybackEnd));
	}
	doc.pushBack(assets);*/

	doc.write(writeFile(sessionPath), JsonTree::WriteOptions());

	return true;
}

void VDSession::restore()
{
	// save load settings
	//load();

	// check to see if json file exists
	if (!fs::exists(sessionPath)) {
		return;
	}

	try {
		JsonTree doc(loadFile(sessionPath));
		/*if (doc.hasChild("shaders")) {
			JsonTree shaders(doc.getChild("shaders"));
			if (shaders.hasChild("0")) createShaderFbo(shaders.getValueForKey<string>("0"));
			if (shaders.hasChild("1")) createShaderFbo(shaders.getValueForKey<string>("1"));
			if (shaders.hasChild("2")) createShaderFbo(shaders.getValueForKey<string>("2"));
			if (shaders.hasChild("3")) createShaderFbo(shaders.getValueForKey<string>("3"));
		}*/
		if (doc.hasChild("settings")) {
			JsonTree settings(doc.getChild("settings"));
			if (settings.hasChild("bpm")) {
				mOriginalBpm = settings.getValueForKey<float>("bpm", 166.0f);
				CI_LOG_W("getBpm" + toString(mVDAnimation->getBpm()) + " mOriginalBpm " + toString(mOriginalBpm));
				mVDAnimation->setBpm(mOriginalBpm);
				CI_LOG_W("getBpm" + toString(mVDAnimation->getBpm()));
			};
			if (settings.hasChild("beatsperbar")) mVDAnimation->setIntUniformValueByName("iBeatsPerBar", settings.getValueForKey<int>("beatsperbar"));
			if (mVDAnimation->getIntUniformValueByName("iBeatsPerBar") < 1) mVDAnimation->setIntUniformValueByName("iBeatsPerBar", 4);
			//if (settings.hasChild("fadeindelay")) mFadeInDelay = settings.getValueForKey<int>("fadeindelay");
			//if (settings.hasChild("fadeoutdelay")) mFadeOutDelay = settings.getValueForKey<int>("fadeoutdelay");
			if (settings.hasChild("endframe")) mVDAnimation->mEndFrame = settings.getValueForKey<int>("endframe");
			CI_LOG_W("getBpm" + toString(mVDAnimation->getBpm()) + " mTargetFps " + toString(mTargetFps));
			mTargetFps = mVDAnimation->getBpm() / 60.0f * mFpb;
			CI_LOG_W("getBpm" + toString(mVDAnimation->getBpm()) + " mTargetFps " + toString(mTargetFps));
		}

		/*if (doc.hasChild("assets")) {
			JsonTree assets(doc.getChild("assets"));
			if (assets.hasChild("wavefile")) mWaveFileName = assets.getValueForKey<string>("wavefile");
			if (assets.hasChild("waveplaybackdelay")) mWavePlaybackDelay = assets.getValueForKey<int>("waveplaybackdelay");
			if (assets.hasChild("moviefile")) mMovieFileName = assets.getValueForKey<string>("moviefile");
			if (assets.hasChild("movieplaybackdelay")) mMoviePlaybackDelay = assets.getValueForKey<int>("movieplaybackdelay");
			if (assets.hasChild("imagesequencepath")) mImageSequencePath = assets.getValueForKey<string>("imagesequencepath");
			if (assets.hasChild("text")) mText = assets.getValueForKey<string>("text");
			if (assets.hasChild("textplaybackdelay")) mTextPlaybackDelay = assets.getValueForKey<int>("textplaybackdelay");
			if (assets.hasChild("textplaybackend")) mTextPlaybackEnd = assets.getValueForKey<int>("textplaybackend");
		}*/

	}
	catch (const JsonTree::ExcJsonParserError& exc) {
		CI_LOG_W(exc.what());
	}
}

void VDSession::resetSomeParams() {
	// parameters not exposed in json file
	mFpb = 16;
	mVDAnimation->setBpm(mOriginalBpm);
	mTargetFps = mOriginalBpm / 60.0f * mFpb;
}

void VDSession::reset()
{
	// parameters exposed in json file
	mOriginalBpm = 166;
	/* TODO 20200221
	mWaveFileName = "";
	mWavePlaybackDelay = 10;
	mMovieFileName = "";
	mImageSequencePath = "";
	mMoviePlaybackDelay = 10;
	mFadeInDelay = 5;
	mFadeOutDelay = 1;
	mText = "";
	mTextPlaybackDelay = 10;
	mTextPlaybackEnd = 2020000;*/
	mVDAnimation->mEndFrame = 20000000;

	resetSomeParams();
}

void VDSession::blendRenderEnable(bool render) {
	mVDAnimation->blendRenderEnable(render);
}

void VDSession::fileDrop(FileDropEvent event) {
	std::string ext = "";
	//string fileName = "";

	unsigned int index = (int)(event.getX() / (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin));
	int y = (int)(event.getY());
	//if (index < 2 || y < mVDSettings->uiYPosRow3 || y > mVDSettings->uiYPosRow3 + mVDSettings->uiPreviewH) index = 0;
	ci::fs::path mPath = event.getFile(event.getNumFiles() - 1);
	std::string absolutePath = mPath.string();
	// use the last of the dropped files
	int dotIndex = absolutePath.find_last_of(".");
	int slashIndex = absolutePath.find_last_of("\\");

	if (dotIndex != std::string::npos && dotIndex > slashIndex) {
		ext = absolutePath.substr(dotIndex + 1);
		//fileName = absolutePath.substr(slashIndex + 1, dotIndex - slashIndex - 1);
		if (ext == "json") {
			JsonTree json(loadFile(absolutePath));
			fboFromJson(json);
		}
		else if (ext == "glsl" || ext == "frag" || ext == "fs") {
			loadFragmentShader(absolutePath, index);

			// façade fbo()->shader()->uniform()->execute() 
		}
		else if (ext == "png" || ext == "jpg") {
			if (index < 1) index = 1;
			if (index > 3) index = 3;
			loadImageFile(absolutePath, index);
		}
		/*else if (ext == "wav" || ext == "mp3") {
			loadAudioFile(absolutePath);
		}
		else if (ext == "xml") {
		}
		else if (ext == "mov") {
			loadMovie(absolutePath, index);
		}
		else if (ext == "txt") {
		}
		else if (ext == "") {
			// try loading image sequence from dir
			if (!loadImageSequence(absolutePath, index)) {
				// try to load a folder of shaders
				loadShaderFolder(absolutePath);
			}
		}*/
	}
}

#pragma region events
bool VDSession::handleMouseMove(MouseEvent &event)
{
	bool handled = true;
	// 20180318 handled in VDUIMouse mVDAnimation->setVec4UniformValueByIndex(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	// pass this mouse event to the warp editor first
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDown(MouseEvent &event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	// 20180318 handled in VDUIMouse mVDAnimation->setVec4UniformValueByIndex(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseDrag(MouseEvent &event)
{

	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}
	// 20180318 handled in VDUIMouse mVDAnimation->setVec4UniformValueByIndex(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleMouseUp(MouseEvent &event)
{
	bool handled = true;
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarpList, event)) {
		// let your application perform its mouseMove handling here
		handled = false;
	}

	// 20180318 handled in VDUIMouse mVDAnimation->setVec4UniformValueByIndex(70, vec4(event.getX(), event.getY(), event.isLeftDown(), event.isRightDown()));
	event.setHandled(handled);
	return event.isHandled();
}


bool VDSession::handleKeyDown(KeyEvent &event)
{
	bool handled = true;
	float newValue;
#if defined( CINDER_COCOA )
	bool isModDown = event.isMetaDown();
#else // windows
	bool isModDown = event.isControlDown();
#endif
	bool isShiftDown = event.isShiftDown();
	bool isAltDown = event.isAltDown();
	CI_LOG_V("session keydown: " + toString(event.getCode()) + " ctrl: " + toString(isModDown) + " shift: " + toString(isShiftDown) + " alt: " + toString(isAltDown));
	// pass this key event to the warp editor first
	if (!Warp::handleKeyDown(mWarpList, event)) {
		// pass this event to Mix handler
		if (!mVDAnimation->handleKeyDown(event)) {
			switch (event.getCode()) {
			case KeyEvent::KEY_w:
				CI_LOG_V("oscConnect");
				if (isModDown) {
					//oscConnect();
				}
				else {
					// handled in main app
					//handled = false;
					// toggle warp edit mode
					Warp::enableEditMode(!Warp::isEditModeEnabled());
				}
				break;

			case KeyEvent::KEY_F1:
				mMode = 0;
				break;
			case KeyEvent::KEY_F2:
				mMode = 1;
				break;
			case KeyEvent::KEY_F3:
				mMode = 2;
				break;
			case KeyEvent::KEY_F4:
				mMode = 3;
				break;
			case KeyEvent::KEY_F5:
				mMode = 4;
				break;
			case KeyEvent::KEY_F6:
				mMode = 5;
				break;
			case KeyEvent::KEY_F7:
				mMode = 6;
				break;
			case KeyEvent::KEY_F8:
				mMode = 7;
				break;
			case KeyEvent::KEY_F9:
				mMode = 8;
				break;
				//case KeyEvent::KEY_SPACE:
				//mVDTextures->playMovie();
				//mVDAnimation->currentScene++;
				//if (mMovie) { if (mMovie->isPlaying()) mMovie->stop(); else mMovie->play(); }
				//break;
			//case KeyEvent::KEY_0:
				//break;
			//case KeyEvent::KEY_l:
				// live params TODO mVDAnimation->load();
				//mLoopVideo = !mLoopVideo;
				//if (mMovie) mMovie->setLoop(mLoopVideo);
				//break;
			case KeyEvent::KEY_x:
				// trixels
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->ITRIXELS, mVDAnimation->getUniformValue(mVDSettings->ITRIXELS) + 0.05f);
				break;
			case KeyEvent::KEY_r:
				if (isAltDown) {
					//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IBR, mVDAnimation->getUniformValue(mVDSettings->IBR), false, true, isShiftDown, isModDown);
				}
				else {
					//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IFR, mVDAnimation->getUniformValue(mVDSettings->IFR), false, true, isShiftDown, isModDown);
				}
				break;
			case KeyEvent::KEY_g:
				if (isAltDown) {
					//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IBG, mVDAnimation->getUniformValue(mVDSettings->IBG), false, true, isShiftDown, isModDown);
				}
				else {
					//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IFG, mVDAnimation->getUniformValue(mVDSettings->IFG), false, true, isShiftDown, isModDown);
				}
				break;
			case KeyEvent::KEY_b:
				if (isAltDown) {
					//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IBB, mVDAnimation->getUniformValue(mVDSettings->IBB), false, true, isShiftDown, isModDown);
				}
				else {
					//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IFB, mVDAnimation->getUniformValue(mVDSettings->IFB), false, true, isShiftDown, isModDown);
				}
				break;
			case KeyEvent::KEY_a:
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IFA, mVDAnimation->getUniformValue(mVDSettings->IFA), false, true, isShiftDown, isModDown);
				break;
			case KeyEvent::KEY_u:
				// chromatic
				// TODO find why can't put value >0.9 or 0.85!
				newValue = mVDAnimation->getUniformValue(mVDSettings->ICHROMATIC) + 0.05f;
				if (newValue > 1.0f) newValue = 0.0f;
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->ICHROMATIC, newValue);
				break;
			case KeyEvent::KEY_p:
				// pixelate
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IPIXELATE, mVDAnimation->getUniformValue(mVDSettings->IPIXELATE) + 0.05f);
				break;
			case KeyEvent::KEY_y:
				// glitch
				//! 20200526 mVDSocketio->changeBoolValue(mVDSettings->IGLITCH, true);
				break;
			case KeyEvent::KEY_i:
				// invert
				//! 20200526 mVDSocketio->changeBoolValue(mVDSettings->IINVERT, true);
				break;
			case KeyEvent::KEY_o:
				// toggle
				//! 20200526 mVDSocketio->changeBoolValue(mVDSettings->ITOGGLE, true);
				break;
			case KeyEvent::KEY_z:
				// zoom
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IZOOM, mVDAnimation->getUniformValue(mVDSettings->IZOOM) - 0.05f);
				break;
				/* removed temp for Sky Project case KeyEvent::KEY_LEFT:
					//mVDTextures->rewindMovie();
					if (mVDAnimation->getUniformValue(21) > 0.1f) mVDSocketio->changeFloatValue(21, mVDAnimation->getUniformValue(21) - 0.1f);
					break;
				case KeyEvent::KEY_RIGHT:
					//mVDTextures->fastforwardMovie();
					if (mVDAnimation->getUniformValue(21) < 1.0f) mVDSocketio->changeFloatValue(21, mVDAnimation->getUniformValue(21) + 0.1f);
					break;*/
			case KeyEvent::KEY_PAGEDOWN:
			case KeyEvent::KEY_RIGHT:
				// crossfade right
				//! 20200526 if (mVDAnimation->getUniformValue(mVDSettings->IXFADE) < 1.0f) mVDSocketio->changeFloatValue(mVDSettings->IXFADE, mVDAnimation->getUniformValue(mVDSettings->IXFADE) + 0.1f);
				break;
			case KeyEvent::KEY_PAGEUP:
			case KeyEvent::KEY_LEFT:
				// crossfade left
				//! 20200526 if (mVDAnimation->getUniformValue(mVDSettings->IXFADE) > 0.0f) mVDSocketio->changeFloatValue(mVDSettings->IXFADE, mVDAnimation->getUniformValue(mVDSettings->IXFADE) - 0.1f);
				break;
			case KeyEvent::KEY_UP:
				// imgseq next
				//incrementSequencePosition();
				break;
			case KeyEvent::KEY_DOWN:
				// imgseq next
				//decrementSequencePosition();
				break;
			case KeyEvent::KEY_v:
				//if (isModDown) fboFlipV(0);// TODO other indexes mVDSettings->mFlipV = !mVDSettings->mFlipV; useless?
				break;
			case KeyEvent::KEY_h:
				if (isModDown) {
					//fboFlipH(0);// TODO other indexes mVDSettings->mFlipH = !mVDSettings->mFlipH; useless?
				}
				else {
					// ui visibility
					toggleUI();
				}
				break;
			case KeyEvent::KEY_d:
				/*if (isAltDown) {
					setSpeed(0, getSpeed(0) - 0.01f);
				}
				else {
					setSpeed(0, getSpeed(0) + 0.01f);
				}*/
				break;
			default:
				CI_LOG_V("session keydown: " + toString(event.getCode()));
				handled = false;
				break;
			}
		}
	}
	CI_LOG_V((handled ? "session keydown handled " : "session keydown not handled "));
	event.setHandled(handled);
	return event.isHandled();
}

bool VDSession::handleKeyUp(KeyEvent &event) {
	bool handled = true;

	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarpList, event)) {
		if (!mVDAnimation->handleKeyUp(event)) {
			// Animation did not handle the key, so handle it here
			switch (event.getCode()) {
			case KeyEvent::KEY_y:
				// glitch
				//! 20200526 mVDSocketio->changeBoolValue(mVDSettings->IGLITCH, false);
				break;
			case KeyEvent::KEY_t:
				// trixels
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->ITRIXELS, 0.0f);
				break;
			case KeyEvent::KEY_i:
				// invert
				//! 20200526 mVDSocketio->changeBoolValue(mVDSettings->IINVERT, false);
				break;
			case KeyEvent::KEY_u:
				// chromatic
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->ICHROMATIC, 0.0f);
				break;
			case KeyEvent::KEY_p:
				// pixelate
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IPIXELATE, 1.0f);
				break;
			case KeyEvent::KEY_o:
				// toggle
				//! 20200526 mVDSocketio->changeBoolValue(mVDSettings->ITOGGLE, false);
				break;
			case KeyEvent::KEY_z:
				// zoom
				//! 20200526 mVDSocketio->changeFloatValue(mVDSettings->IZOOM, 1.0f);
				break;
			default:
				CI_LOG_V("session keyup: " + toString(event.getCode()));
				handled = false;
				break;
			}
		}
	}
	CI_LOG_V((handled ? "session keyup handled " : "session keyup not handled "));
	event.setHandled(handled);
	return event.isHandled();
}
#pragma endregion events
// fbos
#pragma region fbos

#pragma endregion fbos
// shaders

// mix
#pragma region mix


unsigned int VDSession::fboFromJson(const JsonTree &json, unsigned int aFboIndex) {
	unsigned int rtn = 0;

	rtn = createFboShaderTexture(json, aFboIndex);
	return rtn;
}

#pragma endregion mix
