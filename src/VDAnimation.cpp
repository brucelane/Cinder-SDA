#include "VDAnimation.h"

using namespace videodromm;

VDAnimation::VDAnimation(VDSettingsRef aVDSettings) {
	mVDSettings = aVDSettings;
	mVDUniform = VDUniform::create(mVDSettings);
	mBlendRender = false;
	//audio
	mAudioBuffered = false;	mAudioFormat = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
	mAudioTexture = ci::gl::Texture::create(64, 2, mAudioFormat);
	mLineInInitialized = false;
	mWaveInitialized = false;
	mAudioName = "not initialized";
	//setUseLineIn(true);
	maxVolume = 0.0f;
	for (int i = 0; i < 7; i++)
	{
		freqIndexes[i] = i * 7;
	}
	for (int i = 0; i < mFFTWindowSize; i++)
	{
		iFreqs[i] = 0.0f;
	}
	// live json params
	/*mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "live_params.json";
	JsonBag::add(&mBackgroundColor, "background_color");
	JsonBag::add(&mExposure, "exposure", []() {
		app::console() << "Updated exposure" << endl;

	});
	JsonBag::add(&mText, "text", []() {
		app::console() << "Updated text" << endl;
	});
	mAutoBeatAnimation = true;
	JsonBag::add(&mAutoBeatAnimation, "autobeatanimation");*/
	currentScene = 0;

	previousTime = 0.0f;
	counter = 0;
	// tempo
	mUseTimeWithTempo = false;
	// init timer
	mTimer.start();
	startTime = currentTime = mTimer.getSeconds();


	// iRHandX  
	//createFloatUniform("iRHandX", mVDSettings->IRHANDX, 320.0f, 0.0f, 1280.0f);
	//// iRHandY  
	//createFloatUniform("iRHandY", mVDSettings->IRHANDY, 240.0f, 0.0f, 800.0f);
	//// iLHandX  
	//createFloatUniform("iLHandX", mVDSettings->ILHANDX, 320.0f, 0.0f, 1280.0f);
	//// iLHandY  
	//createFloatUniform("iLHandY", mVDSettings->ILHANDY, 240.0f, 0.0f, 800.0f);

	load();
	loadAnimation();

	setVec3UniformValueByIndex(mVDSettings->IRESOLUTION, vec3(getUniformValue(mVDSettings->IRESOLUTIONX), getUniformValue(mVDSettings->IRESOLUTIONY), 1.0));
}

//! uniform to json
JsonTree VDAnimation::uniformToJson(int i)
{
	stringstream svec4;
	JsonTree		json;
	//string s = controlIndexes[i];

	JsonTree u = JsonTree::makeArray("uniform");
	// common
	int uniformType = mVDUniform->getUniformType(i);//  shaderUniforms[i].uniformType;
	u.addChild(ci::JsonTree("type", uniformType));
	u.addChild(ci::JsonTree("name", mVDUniform->getUniformName(i)));
	u.addChild(ci::JsonTree("index", i));
	// type specific 
	switch (uniformType) {
	case 0:
		//float
		u.addChild(ci::JsonTree("value", mVDUniform->getUniformDefaultValue(i)));
		u.addChild(ci::JsonTree("min", mVDUniform->getUniformMinValue(i)));
		u.addChild(ci::JsonTree("max", mVDUniform->getUniformMaxValue(i)));
		break;
	case 1:
		// sampler2d
		u.addChild(ci::JsonTree("textureindex", mVDUniform->getUniformTextureIndex(i)));
		break;
	case 4:
		// vec4
		//svec4 << toString(shaderUniforms[i].vec4Value.x) << "," << toString(shaderUniforms[i].vec4Value.y);
		//svec4 << "," << toString(shaderUniforms[i].vec4Value.z) << "," << toString(shaderUniforms[i].vec4Value.w);
		u.addChild(ci::JsonTree("value", mVDUniform->getUniformDefaultValue(i)));
		break;
	case 5:
		// int
		u.addChild(ci::JsonTree("value", mVDUniform->getUniformDefaultValue(i)));
		break;
	case 6:
		// boolean
		u.addChild(ci::JsonTree("value", mVDUniform->getUniformDefaultValue(i)));
		break;
	default:
		break;
	}

	json.pushBack(u);
	return json;
}
void VDAnimation::saveUniforms()
{
	/*string jName;
	int ctrlSize = math<int>::min(310, controlIndexes.size());
	float jMin, jMax;
	JsonTree		json;
	// create uniforms json
	JsonTree uniformsJson = JsonTree::makeArray("uniforms");

	for (unsigned i = 0; i < ctrlSize; ++i) {
		JsonTree		u(uniformToJson(i));
		// create <uniform>
		uniformsJson.pushBack(u);
	}
	// write file
	json.pushBack(uniformsJson);
	json.write(mUniformsJson);*/
}

/*bool VDAnimation::hasFloatChanged(int aIndex) {
	if (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != controlValues[aIndex]) {
	//CI_LOG_V("hasFloatChanged, aIndex:" + toString(aIndex));
	CI_LOG_V("hasFloatChanged, shaderUniforms[getUniformNameForIndex(aIndex)].floatValue:" + toString(shaderUniforms[getUniformNameForIndex(aIndex)].floatValue));
	CI_LOG_V("hasFloatChanged, controlValues[aIndex]:" + toString(controlValues[aIndex]));
	//CI_LOG_W("hasFloatChanged, getUniformNameForIndex(aIndex):" + toString(getUniformNameForIndex(aIndex)));
	}
	return (shaderUniforms[getUniformNameForIndex(aIndex)].floatValue != controlValues[aIndex]);
	}
bool VDAnimation::toggleValue(unsigned int aIndex) {
	shaderUniforms[aIndex].boolValue = !shaderUniforms[aIndex].boolValue;
	return shaderUniforms[aIndex].boolValue;
}
bool VDAnimation::toggleAuto(unsigned int aIndex) {
	shaderUniforms[aIndex].automatic = !shaderUniforms[aIndex].automatic;
	return shaderUniforms[aIndex].automatic;
}
bool VDAnimation::toggleTempo(unsigned int aIndex) {
	shaderUniforms[aIndex].autotime = !shaderUniforms[aIndex].autotime;
	return shaderUniforms[aIndex].autotime;
}
bool VDAnimation::toggleBass(unsigned int aIndex) {
	shaderUniforms[aIndex].autobass = !shaderUniforms[aIndex].autobass;
	return shaderUniforms[aIndex].autobass;
}
bool VDAnimation::toggleMid(unsigned int aIndex) {
	shaderUniforms[aIndex].automid = !shaderUniforms[aIndex].automid;
	return shaderUniforms[aIndex].automid;
}
bool VDAnimation::toggleTreble(unsigned int aIndex) {
	shaderUniforms[aIndex].autotreble = !shaderUniforms[aIndex].autotreble;
	return shaderUniforms[aIndex].autotreble;
}
void VDAnimation::resetAutoAnimation(unsigned int aIndex) {
	shaderUniforms[aIndex].automatic = false;
	shaderUniforms[aIndex].autotime = false;
	shaderUniforms[aIndex].autobass = false;
	shaderUniforms[aIndex].automid = false;
	shaderUniforms[aIndex].autotreble = false;
	shaderUniforms[aIndex].floatValue = shaderUniforms[aIndex].defaultValue;
}

bool VDAnimation::isExistingUniform(const string& aName) {
	return shaderUniforms[stringToIndex(aName)].isValid;
}
*/
void VDAnimation::load() {
	// Create json file if it doesn't already exist.
/*#if defined( CINDER_MSW )
	if (fs::exists(mJsonFilePath)) {
		bag()->load(mJsonFilePath);
	}
	else {
		bag()->save(mJsonFilePath);
		bag()->load(mJsonFilePath);
	}
#endif*/
}
void VDAnimation::save() {
	saveAnimation();
	saveUniforms();
}
void VDAnimation::saveAnimation() {
	// save 
	/*fs::path mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "animation.json";
	JsonTree doc;
	JsonTree badtv = JsonTree::makeArray("badtv");

	for (const auto& item : mBadTV) {
		if (item.second > 0.0001) badtv.addChild(ci::JsonTree(ci::toString(item.first), ci::toString(item.second)));
	}

	doc.pushBack(badtv);
	doc.write(writeFile(mJsonFilePath), JsonTree::WriteOptions()); */
	// backup save
	/*string fileName = "animation" + toString(getElapsedFrames()) + ".json";
	mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / fileName;
	doc.write(writeFile(mJsonFilePath), JsonTree::WriteOptions());*/
}
void VDAnimation::loadAnimation() {

	/*fs::path mJsonFilePath = app::getAssetPath("") / mVDSettings->mAssetsPath / "animation.json";
	// Create json file if it doesn't already exist.
	if (!fs::exists(mJsonFilePath)) {
		std::ofstream oStream(mJsonFilePath.string());
		oStream.close();
	}
	if (!fs::exists(mJsonFilePath)) {
		return;
	}
	try {
		JsonTree doc(loadFile(mJsonFilePath));
		JsonTree badtv(doc.getChild("badtv"));
		for (JsonTree::ConstIter item = badtv.begin(); item != badtv.end(); ++item) {
			const auto& key = std::stoi(item->getKey());
			const auto& value = item->getValue<float>();
			mBadTV[key] = value;

		}
	}
	catch (const JsonTree::ExcJsonParserError&) {
		CI_LOG_W("Failed to parse json file.");
	} */
}
/*
void VDAnimation::setExposure(float aExposure) {
	mExposure = aExposure;
}
void VDAnimation::setAutoBeatAnimation(bool aAutoBeatAnimation) {
	mAutoBeatAnimation = aAutoBeatAnimation;
}*/
bool VDAnimation::handleKeyDown(KeyEvent &event)
{
	//float newValue;
	/*bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_s:
		// save animation
		save();
		break;
	//case KeyEvent::KEY_u:
	//	// save badtv keyframe
	//	mBadTV[getElapsedFrames() - 10] = 1.0f;
	//	//iBadTvRunning = true;
	//	// duration = 0.2
	//	shaderUniforms["iBadTv"].floatValue = 5.0f;
	//	//timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
	//	break;
	case KeyEvent::KEY_d:
		// save end keyframe
		setEndFrame(getElapsedFrames() - 10);
		break;

		//case KeyEvent::KEY_x:
	case KeyEvent::KEY_y:
		mVDSettings->iXorY = !mVDSettings->iXorY;
		break;

	default:
		handled = false;
	} */

	event.setHandled(false);

	return event.isHandled();
}
bool VDAnimation::handleKeyUp(KeyEvent &event)
{
	bool handled = true;
	switch (event.getCode()) {
	case KeyEvent::KEY_u:
		// save badtv keyframe
		// not used for now mBadTV[getElapsedFrames()] = 0.001f;
		//shaderUniforms["iBadTv"].floatValue = 0.0f;
		break;

	default:
		handled = false;
	}
	event.setHandled(handled);

	return event.isHandled();
}
ci::gl::TextureRef VDAnimation::getAudioTexture() {

	mAudioFormat = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
	auto ctx = audio::Context::master();
	if (!mLineInInitialized) {
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
		if (getUseLineIn()) {
			// linein
			preventLineInCrash(); // at next launch
			CI_LOG_W("trying to open mic/line in, if no line follows in the log, the app crashed so put UseLineIn to false in the VDSettings.xml file");
			mLineIn = ctx->createInputDeviceNode(); //crashes if linein is present but disabled, doesn't go to catch block
			CI_LOG_V("mic/line in opened");
			saveLineIn();
			mAudioName = mLineIn->getDevice()->getName();
			auto scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(mFFTWindowSize * 2).windowSize(mFFTWindowSize);// CHECK is * 2 needed
			mMonitorLineInSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeLineInFmt));
			mLineIn >> mMonitorLineInSpectralNode;
			mLineIn->enable();
			mLineInInitialized = true;
		}
	}
#endif
	if (!mWaveInitialized) {
		if (getUseAudio()) {
			// also initialize wave monitor
			auto scopeWaveFmt = audio::MonitorSpectralNode::Format().fftSize(mFFTWindowSize * 2).windowSize(mFFTWindowSize);// CHECK is * 2 needed
			mMonitorWaveSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeWaveFmt));
			ctx->enable();
			mAudioName = "wave";
			mWaveInitialized = true;
		}
	}
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
	if (getUseLineIn()) {
		mMagSpectrum = mMonitorLineInSpectralNode->getMagSpectrum();
	}
	else {
#endif
		if (getUseAudio()) {
			if (isAudioBuffered()) {
				if (mBufferPlayerNode) {
					mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
				}
			}
			else {
				if (mSamplePlayerNode) {
					mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
					mPosition = mSamplePlayerNode->getReadPosition();
				}
			}
		}
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
	}
#endif
	if (!mMagSpectrum.empty()) {

		maxVolume = 0.0f;//mIntensity
		size_t mDataSize = mMagSpectrum.size();
		if (mDataSize > 0 && mDataSize < mFFTWindowSize) {// TODO 20200221 CHECK was + 1
			float db;
			unsigned char signal[mFFTWindowSize];
			for (size_t i = 0; i < mDataSize; i++) {
				float f = mMagSpectrum[i];
				db = audio::linearToDecibel(f);
				f = db * getUniformValueByName("iAudioMult");
				if (f > maxVolume)
				{
					maxVolume = f;
				}
				iFreqs[i] = f;
				// update iFreq uniforms 
				if (i == getFreqIndex(0)) setFloatUniformValueByName("iFreq0", f);
				if (i == getFreqIndex(1)) setFloatUniformValueByName("iFreq1", f);
				if (i == getFreqIndex(2)) setFloatUniformValueByName("iFreq2", f);
				if (i == getFreqIndex(3)) setFloatUniformValueByName("iFreq3", f);

				if (i < mFFTWindowSize) {
					int ger = f;
					signal[i] = static_cast<unsigned char>(ger);
				}
			}
			// store it as a 512x2 texture
			// 20200222 mAudioTexture = gl::Texture::create(signal, GL_RED, 64, 2, mAudioFormat);
			mAudioTexture = gl::Texture::create(signal, GL_RED, 32, 1, mAudioFormat);
			/* TODO 20200221 useful?
			if (isAudioBuffered() && mBufferPlayerNode) {
				gl::ScopedFramebuffer fbScp(mFbo);
				gl::clear(Color::black());

				mAudioTexture->bind(0);

				//mWaveformPlot.draw();
				// draw the current play position
				mPosition = mBufferPlayerNode->getReadPosition();
				float readPos = (float)mWidth * mPosition / mBufferPlayerNode->getNumFrames();
				gl::color(ColorA(0, 1, 0, 0.7f));
				gl::drawSolidRect(Rectf(readPos - 2, 0, readPos + 2, (float)mHeight));
				mRenderedTexture = mFbo->getColorTexture();
				return mRenderedTexture;
			} */
		}
	}
	else {
		// generate random values
		// 20200222 for (int i = 0; i < 128; ++i) dTexture[i] = (unsigned char)(i);
		// 20200222 mAudioTexture = gl::Texture::create(dTexture, GL_RED, 64, 2, mAudioFormat);
		// get freqs from Speckthor in VDRouter.cpp
		float db;
		unsigned char signal[mFFTWindowSize];
		for (size_t i = 0; i < mFFTWindowSize; i++) {
			float f = iFreqs[i];
			if (f > maxVolume)
			{
				maxVolume = f;
			}
			// update iFreq uniforms 
			if (i == getFreqIndex(0)) setFloatUniformValueByName("iFreq0", f);
			if (i == getFreqIndex(1)) setFloatUniformValueByName("iFreq1", f);
			if (i == getFreqIndex(2)) setFloatUniformValueByName("iFreq2", f);
			if (i == getFreqIndex(3)) setFloatUniformValueByName("iFreq3", f);

			if (i < mFFTWindowSize) {
				int ger = f;
				signal[i] = static_cast<unsigned char>(ger);
			}
		}
		// store it as a 512x2 texture
		// 20200222 mAudioTexture = gl::Texture::create(signal, GL_RED, 64, 2, mAudioFormat);
		mAudioTexture = gl::Texture::create(signal, GL_RED, 32, 1, mAudioFormat);
		mAudioName = "speckthor";
	}

	return mAudioTexture;
};
void VDAnimation::update() {

	if (mBadTV[getElapsedFrames()] == 0) {
		// TODO check shaderUniforms["iBadTv"].floatValue = 0.0f;
	}
	else {
		// duration = 0.2
		//timeline().apply(&mVDSettings->iBadTv, 60.0f, 0.0f, 0.2f, EaseInCubic());
		mVDUniform->setUniformValue(mVDSettings->IBADTV, 5.0f);
	}

	mVDSettings->iChannelTime[0] = getElapsedSeconds();
	mVDSettings->iChannelTime[1] = getElapsedSeconds() - 1;
	mVDSettings->iChannelTime[2] = getElapsedSeconds() - 2;
	mVDSettings->iChannelTime[3] = getElapsedSeconds() - 3;
	// TIME
	if (mUseTimeWithTempo)
	{
		// Ableton Link from openframeworks SocketIO
		mVDUniform->setUniformValue(mVDSettings->ITIME, 
			mVDUniform->getUniformValue(mVDSettings->ITIME) * mVDSettings->iSpeedMultiplier * mVDUniform->getUniformValue(mVDSettings->ITIMEFACTOR));
		//shaderUniforms["iElapsed"].floatValue = shaderUniforms["iPhase"].floatValue * mVDSettings->iSpeedMultiplier * shaderUniforms["iTimeFactor"].floatValue;
		// sos
		// IBARBEAT = IBAR * 4 + IBEAT
		int current = getIntUniformValueByIndex(mVDSettings->IBARBEAT);
		if (current == 426 || current == 428 || current == 442) { mLastBar = 0; } //38 to set iStart
		if (mLastBar != getIntUniformValueByIndex(mVDSettings->IBAR)) {
			mLastBar =getIntUniformValueByIndex(mVDSettings->IBAR);
			//if (mLastBar != 5 && mLastBar != 9 && mLastBar < 113) mVDSettings->iStart = mVDSession->getUniformValue(mVDSettings->ITIME);
			// TODO CHECK
			//if (mLastBar != 107 && mLastBar != 111 && mLastBar < 205) mVDSettings->iStart = mVDSession->getUniformValue(mVDSettings->ITIME);
			if (mLastBar < 419 && mLastBar > 424) { mVDSettings->iStart = getUniformValue(mVDSettings->ITIME); }
		}
	}
	else
	{
		mVDUniform->setUniformValue(mVDSettings->ITIME,
			getElapsedSeconds() * mVDSettings->iSpeedMultiplier * mVDUniform->getUniformValue(mVDSettings->ITIMEFACTOR));

		//shaderUniforms[mVDSettings->ITIME].floatValue = getElapsedSeconds() * mVDSettings->iSpeedMultiplier * shaderUniforms[mVDSettings->ITIMEFACTOR].floatValue;//mVDSettings->iTimeFactor;
		//shaderUniforms["iElapsed"].floatValue = getElapsedSeconds() * mVDSettings->iSpeedMultiplier * shaderUniforms["iTimeFactor"].floatValue;//mVDSettings->iTimeFactor;
	}
	// iResolution
	mVDUniform->setVec3UniformValueByIndex(mVDSettings->IRESOLUTION, vec3(getUniformValue(mVDSettings->IRESOLUTIONX), getUniformValue(mVDSettings->IRESOLUTIONY), 1.0));
	mVDUniform->setVec2UniformValueByIndex(mVDSettings->RESOLUTION, vec2(getUniformValue(mVDSettings->IRESOLUTIONX), getUniformValue(mVDSettings->IRESOLUTIONY)));
	mVDUniform->setVec2UniformValueByIndex(mVDSettings->RENDERSIZE, vec2(getUniformValue(mVDSettings->IRESOLUTIONX), getUniformValue(mVDSettings->IRESOLUTIONY)));	
	
	// iDate
	time_t now = time(0);
	tm *   t = gmtime(&now);
	mVDUniform->setVec4UniformValueByIndex(mVDSettings->IDATE,
		vec4(float(t->tm_year + 1900), float(t->tm_mon + 1), float(t->tm_mday), float(t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec)));

#pragma region animation

	currentTime = mTimer.getSeconds();
	// TODO check bounds
	//if (mAutoBeatAnimation) mVDSettings->liveMeter = maxVolume * 2;

	int time = (currentTime - startTime)*1000000.0;

	int elapsed = mVDUniform->getUniformValue( mVDSettings->IDELTATIME) * 1000000.0;
	int elapsedBeatPerBar = mVDUniform->getUniformValue(mVDSettings->IDELTATIME) / 
		(mVDUniform->getUniformValue(mVDSettings->IBEATSPERBAR) + 1)*1000000.0;
	/*if (elapsedBeatPerBar > 0)
	{
		double moduloBeatPerBar = (time % elapsedBeatPerBar) / 1000000.0;
		iTempoTimeBeatPerBar = (float)moduloBeatPerBar;
		if (iTempoTimeBeatPerBar < previousTimeBeatPerBar)
		{
			if (shaderUniforms["iPhase"].intValue > shaderUniforms["iBeatsPerBar"].intValue ) shaderUniforms["iPhase"].intValue = 0;
			shaderUniforms["iPhase"].intValue++;
		}
		previousTimeBeatPerBar = iTempoTimeBeatPerBar;
	} */
	if (elapsed > 0)
	{
		double modulo = (time % elapsed) / 1000000.0;
		mVDUniform->setUniformValue(mVDSettings->ITEMPOTIME, (float)abs(modulo));

		/* not used shaderUniforms["iDeltaTime"].floatValue = (float)abs(modulo);
		if (shaderUniforms["iTempoTime"].floatValue < previousTime)
		{
			//iBar++;
			//if (mAutoBeatAnimation) mVDSettings->iPhase++;
		}*/
		previousTime = mVDUniform->getUniformValue(mVDSettings->ITEMPOTIME);

		// TODO (modulo < 0.1) ? tempoMvg->setNameColor(ColorA::white()) : tempoMvg->setNameColor(UIController::DEFAULT_NAME_COLOR);
		/*for (unsigned int anim = 1; anim < 29; anim++)
		{
			if (shaderUniforms[anim].autotime)
			{
				setUniformValue(anim, (modulo < 0.1) ? shaderUniforms[anim].maxValue : shaderUniforms[anim].minValue);
			}
			else
			{
				if (shaderUniforms[anim].automatic) {
					setUniformValue(anim, lmap<float>(shaderUniforms[mVDSettings->ITEMPOTIME].floatValue, 0.00001, getUniformValue(mVDSettings->IDELTATIME), shaderUniforms[anim].minValue, shaderUniforms[anim].maxValue));
				}
				else
				{
					if (shaderUniforms[anim].autobass) {
						setUniformValue(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getUniformValue(mVDSettings->IFREQ0) / 25.0f);
					}
					else
					{
						if (shaderUniforms[anim].automid) {
							setUniformValue(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getUniformValue(mVDSettings->IFREQ1) / 5.0f);
						}
						else
						{
							if (shaderUniforms[anim].autotreble) {
								setUniformValue(anim, (getFloatUniformDefaultValueByIndex(anim) + 0.01f) * getUniformValue(mVDSettings->IFREQ2) / 2.0f);
							}
						}
					}
				}
			}
		}*/

		// foreground color vec3 update
		mVDUniform->setVec3UniformValueByIndex(mVDSettings->ICOLOR, vec3(getUniformValue(mVDSettings->IFR), getUniformValue(mVDSettings->IFG), getUniformValue(mVDSettings->IFB)));

		// background color vec3 update
		mVDUniform->setVec3UniformValueByIndex(mVDSettings->IBACKGROUNDCOLOR, vec3(getUniformValue(mVDSettings->IBR), getUniformValue(mVDSettings->IBG), getUniformValue(mVDSettings->IBB)));

		// mouse vec4 update
		mVDUniform->setVec4UniformValueByIndex(mVDSettings->IMOUSE, vec4(getUniformValue(mVDSettings->IMOUSEX), getUniformValue(mVDSettings->IMOUSEY), getUniformValue(mVDSettings->IMOUSEZ), 0.0f));

		// TODO migrate:
		if (mVDSettings->autoInvert)
		{
			setBoolUniformValueByIndex(mVDSettings->IINVERT, (modulo < 0.1) ? 1.0 : 0.0);
		}

		/*if (mVDSettings->tEyePointZ)
		{
			mVDSettings->mCamEyePointZ = (modulo < 0.1) ? mVDSettings->minEyePointZ : mVDSettings->maxEyePointZ;
		}
		else
		{
			mVDSettings->mCamEyePointZ = mVDSettings->autoEyePointZ ? lmap<float>(shaderUniforms[mVDSettings->ITEMPOTIME].floatValue, 0.00001, getUniformValue(mVDSettings->IDELTATIME), mVDSettings->minEyePointZ, mVDSettings->maxEyePointZ) : mVDSettings->mCamEyePointZ;
		}*/

	}
#pragma endregion animation
}

// tempo
void VDAnimation::tapTempo()
{
	startTime = currentTime = mTimer.getSeconds();

	mTimer.stop();
	mTimer.start();

	// check for out of time values - less than 50% or more than 150% of from last "TAP and whole time buffer is going to be discarded....
	if (counter > 2 && (buffer.back() * 1.5 < currentTime || buffer.back() * 0.5 > currentTime))
	{
		buffer.clear();
		counter = 0;
		averageTime = 0;
	}
	if (counter >= 1)
	{
		buffer.push_back(currentTime);
		calculateTempo();
	}
	counter++;
}
void VDAnimation::calculateTempo()
{
	// NORMAL AVERAGE
	double tAverage = 0;
	for (int i = 0; i < buffer.size(); i++)
	{
		tAverage += buffer[i];
	}
	averageTime = (double)(tAverage / buffer.size());
	setUniformValue(mVDSettings->IDELTATIME, averageTime);
	setBpm(60 / averageTime);
}

void VDAnimation::preventLineInCrash() {
	setUseLineIn(false);
	mVDSettings->save();
}
void VDAnimation::saveLineIn() {
	setUseLineIn(true);
	mVDSettings->save();
}