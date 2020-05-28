/*
	VDAnimation
	Handles all uniform variables for shaders: index, name, type, bounds and their animation.
*/
// TODO put audio in a separate class
// TODO put timing in separate class?
// TODO Implement a double map to replace map<int, string> controlIndexes and map<string, VDUniform> shaderUniforms
// TODO remove struct
// TODO implement lazy loading for audio

#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
//!  audio
#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"
#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/Rand.h"
//!  json
//#include "cinder/Json.h"
//!  Settings
#include "VDSettings.h"
//!  Uniforms
#include "VDUniform.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDAnimation instance
	typedef std::shared_ptr<class VDAnimation> VDAnimationRef;


	class VDAnimation {
	public:
		VDAnimation(VDSettingsRef aVDSettings);

		static VDAnimationRef			create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDAnimation>(new VDAnimation(aVDSettings));
		}
		void							update();
		void							load();
		void							save();
		string							getAssetsPath() {
			return mVDSettings->mAssetsPath;
		}
		/*Color							getBackgroundColor() { return mBackgroundColor; };
		float							getExposure() { return mExposure; };
		bool							getAutoBeatAnimation() { return mAutoBeatAnimation; };
		void							setExposure(float aExposure);
		void							setAutoBeatAnimation(bool aAutoBeatAnimation);*/

		const int						mBlendModes = 28;
		void							blendRenderEnable(bool render) { mBlendRender = render; };
		// tap tempo
		bool							mUseTimeWithTempo;
		void							toggleUseTimeWithTempo() { mUseTimeWithTempo = !mUseTimeWithTempo; };
		void							useTimeWithTempo() { mUseTimeWithTempo = true; };
		bool							getUseTimeWithTempo() { return mUseTimeWithTempo; };
		float							iTempoTimeBeatPerBar;
		float							getBpm() {
			return getFloatUniformValueByIndex(mVDSettings->IBPM);
		};
		void							setBpm(float aBpm) {
			if (aBpm > 0.0f) {
				setFloatUniformValueByIndex(mVDSettings->IBPM, aBpm);
				setFloatUniformValueByIndex(mVDSettings->IDELTATIME, 60 / aBpm);
			}
		};
		void							tapTempo();
		int								getEndFrame() { return mEndFrame; };
		void							setEndFrame(int frame) { mEndFrame = frame; };

		// animation
		int								currentScene;
		//int							getBadTV(int frame);
		// keyboard
		bool							handleKeyDown(KeyEvent& event);
		bool							handleKeyUp(KeyEvent& event);
		// audio
		ci::gl::TextureRef				getAudioTexture();
		string							getAudioTextureName() { return mAudioName; };
		float							maxVolume;
		bool							mLineInInitialized;
		bool							mWaveInitialized;
		audio::InputDeviceNodeRef		mLineIn;
		audio::MonitorSpectralNodeRef	mMonitorLineInSpectralNode;
		audio::MonitorSpectralNodeRef	mMonitorWaveSpectralNode;
		audio::SamplePlayerNodeRef		mSamplePlayerNode;
		audio::SourceFileRef			mSourceFile;
		audio::MonitorSpectralNodeRef	mScopeLineInFmt;
		audio::BufferPlayerNodeRef		mBufferPlayerNode;

		vector<float>					mMagSpectrum;

		// number of frequency bands of our spectrum
		static const int				mFFTWindowSize = 32; // fft window size 20200222 was 128;
		float							iFreqs[mFFTWindowSize];
		int								mPosition;
		string							mAudioName;
		void							preventLineInCrash(); // at next launch
		void							saveLineIn();
		bool							getUseAudio() {
			return mVDSettings->mUseAudio;
		};
		bool							getUseLineIn() {
			return mVDSettings->mUseLineIn;
		};
		void							setUseLineIn(bool useLineIn) {
			mVDSettings->mUseLineIn = useLineIn;
		};
		void							toggleUseLineIn() { mVDSettings->mUseLineIn = !mVDSettings->mUseLineIn; };

		// audio
		bool							isAudioBuffered() { return mAudioBuffered; };
		void							toggleAudioBuffered() { mAudioBuffered = !mAudioBuffered; };

		// shaders
		//bool							isExistingUniform(const string& aName);
		int								getUniformType(const string& aName);

		/*string							getUniformNameForIndex(int aIndex) {
			return shaderUniforms[aIndex].name; //controlIndexes[aIndex];
		};*/
		int								getUniformIndexForName(const string& aName) {
			return mVDUniform->getUniformIndexForName(aName);
			//return shaderUniforms[stringToIndex(aName)].index;
		};
		bool							toggleAuto(unsigned int aIndex);
		bool							toggleValue(unsigned int aIndex);
		bool							toggleTempo(unsigned int aIndex);
		bool							toggleBass(unsigned int aIndex);
		bool							toggleMid(unsigned int aIndex);
		bool							toggleTreble(unsigned int aIndex);
		void							resetAutoAnimation(unsigned int aIndex);
		bool							setFloatUniformValueByIndex(unsigned int aIndex, float aValue);

		bool							setBoolUniformValueByIndex(unsigned int aIndex, bool aValue) {
			return mVDUniform->setBoolUniformValueByIndex(aIndex, aValue);
		}
		void							setIntUniformValueByName(const string& aName, int aValue) {
			mVDUniform->setIntUniformValueByName(aName, aValue);
		};
		void							setIntUniformValueByIndex(unsigned int aIndex, int aValue) {
			mVDUniform->setIntUniformValueByIndex(aIndex, aValue);
		}
		void							setFloatUniformValueByName(const string& aName, float aValue) {
			mVDUniform->setFloatUniformValueByName(aName, aValue);
		}
		void setVec2UniformValueByName(const string& aName, vec2 aValue) {
			mVDUniform->setVec2UniformValueByName(aName, aValue);
		}
		void setVec2UniformValueByIndex(unsigned int aIndex, vec2 aValue) {
			mVDUniform->setVec2UniformValueByIndex(aIndex, aValue);
		}
		void setVec3UniformValueByName(const string& aName, vec3 aValue) {
			mVDUniform->setVec3UniformValueByName(aName, aValue);
		}
		void setVec3UniformValueByIndex(unsigned int aIndex, vec3 aValue) {
			mVDUniform->setVec3UniformValueByIndex(aIndex, aValue);
		}
		void setVec4UniformValueByName(const string& aName, vec4 aValue) {
			mVDUniform->setVec4UniformValueByName(aName, aValue);
		}
		void setVec4UniformValueByIndex(unsigned int aIndex, vec4 aValue) {
			mVDUniform->setVec4UniformValueByIndex(aIndex, aValue);
		}
		bool							getBoolUniformValueByIndex(unsigned int aIndex) {
			return mVDUniform->getBoolUniformValueByIndex(aIndex);
		}
		float							getMinUniformValueByIndex(unsigned int aIndex) {
			return mVDUniform->getMinUniformValueByIndex(aIndex);
		}
		float							getMaxUniformValueByIndex(unsigned int aIndex) {
			return mVDUniform->getMaxUniformValueByIndex(aIndex);
		}
		float							getMinUniformValueByName(const string& aName) {
			return mVDUniform->getMinUniformValueByName(aName);
		}
		float							getMaxUniformValueByName(const string& aName) {
			return mVDUniform->getMaxUniformValueByName(aName);
		}
		bool							getBoolUniformValueByName(const string& aName) {
			return mVDUniform->getBoolUniformValueByName(aName);
		}
		/*vec2							getVec2UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].vec2Value;
		};
		vec3							getVec3UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].vec3Value;
		};
		vec4							getVec4UniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].vec4Value;
		};*/
		float							getFloatUniformValueByIndex(unsigned int aIndex) {
			return mVDUniform->getFloatUniformValueByIndex(aIndex);
		}
		float							getFloatUniformDefaultValueByIndex(unsigned int aIndex) {
			return mVDUniform->getFloatUniformDefaultValueByIndex(aIndex);
		}
		int								getIntUniformValueByIndex(unsigned int aIndex) {
			return mVDUniform->getIntUniformValueByIndex(aIndex);
		}
		int								getSampler2DUniformValueByName(const string& aName) {
			return mVDUniform->getSampler2DUniformValueByName(aName);
		}
		float							getFloatUniformValueByName(const string& aName) {		
				return mVDUniform->getFloatUniformValueByName(aName);		
		}
		
		vec2							getVec2UniformValueByName(const string& aName) {
			return mVDUniform->getVec2UniformValueByName(aName);
		}
		vec3							getVec3UniformValueByName(const string& aName) {
			return mVDUniform->getVec3UniformValueByName(aName);
		}
		vec4							getVec4UniformValueByName(const string& aName) {
			return mVDUniform->getVec4UniformValueByName(aName);
		}
		int								getIntUniformValueByName(const string& aName) {
			return mVDUniform->getIntUniformValueByName(aName);
		};

		// mix fbo
		bool							isFlipH() { return getBoolUniformValueByIndex(mVDSettings->IFLIPH); };
		bool							isFlipV() { return getBoolUniformValueByIndex(mVDSettings->IFLIPV); };
		void							flipH() { setBoolUniformValueByIndex(mVDSettings->IFLIPH, !getBoolUniformValueByIndex(mVDSettings->IFLIPH)); };
		void							flipV() { setBoolUniformValueByIndex(mVDSettings->IFLIPV, !getBoolUniformValueByIndex(mVDSettings->IFLIPV)); };

		unsigned int					getBlendModesCount() { return mBlendModes; };
		bool							renderBlend() { return mBlendRender; };

		// timed animation
		int								mEndFrame;
		int								getFreqIndexSize() { return freqIndexes.size(); };
		int								getFreqIndex(unsigned int aFreqIndex) { return freqIndexes[aFreqIndex]; };
		void							setFreqIndex(unsigned int aFreqIndex, unsigned int aFreq) { freqIndexes[aFreqIndex] = aFreq; };
		//float							getFreq(unsigned int aFreqIndex) { return iFreqs[freqIndexes[aFreqIndex]]; };
		// public for hydra
		void							createFloatUniform(const string& aName, int aCtrlIndex, float aValue = 0.01f, float aMin = 0.0f, float aMax = 1.0f);
		void							createSampler2DUniform(const string& aName, int aCtrlIndex, int aTextureIndex = 0);
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		VDUniformRef					mVDUniform;
		map<int, int>					freqIndexes;
		bool							mAudioBuffered;
		ci::gl::TextureRef				mAudioTexture;
		gl::Texture2d::Format			mAudioFormat;
		unsigned char					dTexture[256];// MUST be < mWindowSize
		// Live json params
		/*fs::path						mJsonFilePath;
		Parameter<Color>				mBackgroundColor;
		Parameter<float>				mExposure;
		Parameter<string>				mText;
		Parameter<bool>					mAutoBeatAnimation;*/
		// shaders

		//shaderUniforms[mVDSettings->ITIME].floatValue = 0.3f;
		//shaderUniforms[StringToIndex("ITIME")] = ...;



		
		/* private in vduniform 
		void							createVec2Uniform(const string& aName, int aCtrlIndex, vec2 aValue = vec2(0.0)) {
			mVDUniform->createVec2Uniform(aName, aCtrlIndex, aValue);
		};
		void							createVec3Uniform(const string& aName, int aCtrlIndex, vec3 aValue = vec3(0.0)) {
			mVDUniform->createVec3Uniform(aName, aCtrlIndex, aValue);
		};
		void							createVec4Uniform(const string& aName, int aCtrlIndex, vec4 aValue = vec4(0.0)) {
			mVDUniform->createVec4Uniform(aName, aCtrlIndex, aValue);
		};
		void							createIntUniform(const string& aName, int aCtrlIndex, int aValue = 1) {
			mVDUniform->createIntUniform(aName, aCtrlIndex, aValue);
		};
		void							createBoolUniform(const string& aName, int aCtrlIndex, bool aValue = false) {
			mVDUniform->createBoolUniform(aName, aCtrlIndex, aValue);
		};*/
		//! write a uniforms json file
		void							saveUniforms();
		ci::JsonTree					uniformToJson(int i);

		// time
		ci::Timer						mTimer;
		std::deque <double>				buffer;
		void							calculateTempo();
		int								counter;
		double							averageTime;
		double							currentTime;
		double							startTime;
		float							previousTime;
		float							previousTimeBeatPerBar;
		JsonTree						mData;
		void							loadAnimation();
		void							saveAnimation();
		int								mLastBar = 0;
		std::unordered_map<int, float>	mBadTV;
		bool							mBlendRender;
		//map<int, string>				controlIndexes; // clé=0=ITIME de  iTime
				//map<string, VDUniform>			shaderUniforms;
				// multiindex de boost
		/*map<int, VDUniform>				shaderUniforms;
		int stringToIndex(const string& key) {
			int rtn = 0;
			if (key == "iTime") {
				rtn = mVDSettings->ITIME;
			}
			else if (key == "iTimeFactor") {
				rtn = mVDSettings->ITIMEFACTOR;
			} // TODO others
			return rtn;
		}*/
	};
}
