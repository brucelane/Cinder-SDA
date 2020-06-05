/*
	VDAnimation
	Handles all uniform variables for shaders: index, name, type, bounds and their animation.
*/
// TODO put audio in a separate class
// TODO put timing in separate class?
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
//using namespace std;

namespace videodromm
{
	// stores the pointer to the VDAnimation instance
	typedef std::shared_ptr<class VDAnimation> VDAnimationRef;

	class VDAnimation {
	public:
		VDAnimation(VDSettingsRef aVDSettings);

		static VDAnimationRef			create(VDSettingsRef aVDSettings)
		{
			return std::shared_ptr<VDAnimation>(new VDAnimation(aVDSettings));
		}
		void							update();
		void							load();
		void							save();
		std::string							getAssetsPath() {
			return mVDSettings->mAssetsPath;
		}

		const int						mBlendModes = 28;
		void							blendRenderEnable(bool render) { mBlendRender = render; };
		// tap tempo
		bool							mUseTimeWithTempo;
		void							toggleUseTimeWithTempo() { mUseTimeWithTempo = !mUseTimeWithTempo; };
		void							useTimeWithTempo() { mUseTimeWithTempo = true; };
		bool							getUseTimeWithTempo() { return mUseTimeWithTempo; };
		float							iTempoTimeBeatPerBar;
		
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
		std::string						getAudioTextureName() { return mAudioName; };
		//float							maxVolume;
		bool							mLineInInitialized;
		bool							mWaveInitialized;
		audio::InputDeviceNodeRef		mLineIn;
		audio::MonitorSpectralNodeRef	mMonitorLineInSpectralNode;
		audio::MonitorSpectralNodeRef	mMonitorWaveSpectralNode;
		audio::SamplePlayerNodeRef		mSamplePlayerNode;
		audio::SourceFileRef			mSourceFile;
		audio::MonitorSpectralNodeRef	mScopeLineInFmt;
		audio::BufferPlayerNodeRef		mBufferPlayerNode;

		std::vector<float>				mMagSpectrum;

		// number of frequency bands of our spectrum
		static const int				mFFTWindowSize = 32; // fft window size 20200222 was 128;
		float							iFreqs[mFFTWindowSize];
		int								mPosition;
		std::string						mAudioName;
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
		int								getUniformTypeByName(const std::string& aName) {
			return mVDUniform->getUniformTypeByName(aName);
		}
		bool							isExistingUniform(const std::string& aName) { return true; }; // TODO
		
		/*
		string							getUniformNameForIndex(int aIndex) {
			return shaderUniforms[aIndex].name; //controlIndexes[aIndex];
		};*/
		int								getUniformIndexForName(const std::string& aName) {
			return mVDUniform->getUniformIndexForName(aName);
			//return shaderUniforms[stringToIndex(aName)].index;
		};
		bool							toggleValue(unsigned int aIndex);

		void							setAnim(unsigned int aCtrl, unsigned int aAnim) {
			mVDUniform->setAnim(aCtrl, aAnim);
		}
		bool							setUniformValue(unsigned int aIndex, float aValue) {
			if (aIndex == mVDSettings->IBPM) {
				if (aValue > 0.0f) {
					mVDUniform->setUniformValue(mVDSettings->IDELTATIME, 60 / aValue);
				}
			}
			return mVDUniform->setUniformValue(aIndex, aValue);
		}

		bool							setBoolUniformValueByIndex(unsigned int aIndex, bool aValue) {
			return mVDUniform->setBoolUniformValueByIndex(aIndex, aValue);
		}
		/*void							setIntUniformValueByName(const std::string& aName, int aValue) {
			mVDUniform->setIntUniformValueByName(aName, aValue);
		};
		void							setIntUniformValueByIndex(unsigned int aIndex, int aValue) {
			mVDUniform->setIntUniformValueByIndex(aIndex, aValue);
		}*/
		void							setFloatUniformValueByName(const std::string& aName, float aValue) {
			mVDUniform->setFloatUniformValueByName(aName, aValue);
		}
		void setVec2UniformValueByName(const std::string& aName, vec2 aValue) {
			mVDUniform->setVec2UniformValueByName(aName, aValue);
		}
		void setVec2UniformValueByIndex(unsigned int aIndex, vec2 aValue) {
			mVDUniform->setVec2UniformValueByIndex(aIndex, aValue);
		}
		void setVec3UniformValueByName(const std::string& aName, vec3 aValue) {
			mVDUniform->setVec3UniformValueByName(aName, aValue);
		}
		void setVec3UniformValueByIndex(unsigned int aIndex, vec3 aValue) {
			mVDUniform->setVec3UniformValueByIndex(aIndex, aValue);
		}
		void setVec4UniformValueByName(const std::string& aName, vec4 aValue) {
			mVDUniform->setVec4UniformValueByName(aName, aValue);
		}
		void setVec4UniformValueByIndex(unsigned int aIndex, vec4 aValue) {
			mVDUniform->setVec4UniformValueByIndex(aIndex, aValue);
		}
		bool							getBoolUniformValueByIndex(unsigned int aIndex) {
			return mVDUniform->getBoolUniformValueByIndex(aIndex);
		}
		float							getMinUniformValue(unsigned int aIndex) {
			return mVDUniform->getMinUniformValue(aIndex);
		}
		float							getMaxUniformValue(unsigned int aIndex) {
			return mVDUniform->getMaxUniformValue(aIndex);
		}
		float							getMinUniformValueByName(const std::string& aName) {
			return mVDUniform->getMinUniformValueByName(aName);
		}
		float							getMaxUniformValueByName(const std::string& aName) {
			return mVDUniform->getMaxUniformValueByName(aName);
		}
		bool							getBoolUniformValueByName(const std::string& aName) {
			return mVDUniform->getBoolUniformValueByName(aName);
		}
		float							getUniformValue(unsigned int aIndex) {
			return mVDUniform->getUniformValue(aIndex);
		}
		std::string						getUniformName(unsigned int aIndex) {
			return mVDUniform->getUniformName(aIndex);
		}
		int								getUniformAnim(unsigned int aIndex) {
			return mVDUniform->getUniformAnim(aIndex);
		}
		float							getFloatUniformDefaultValueByIndex(unsigned int aIndex) {
			return mVDUniform->getFloatUniformDefaultValueByIndex(aIndex);
		}
		int								getIntUniformValueByIndex(unsigned int aIndex) {
			return mVDUniform->getIntUniformValueByIndex(aIndex);
		}
		int								getSampler2DUniformValueByName(const std::string& aName) {
			return mVDUniform->getSampler2DUniformValueByName(aName);
		}
		float							getUniformValueByName(const std::string& aName) {
				return mVDUniform->getUniformValueByName(aName);		
		}
		
		vec2							getVec2UniformValueByName(const std::string& aName) {
			return mVDUniform->getVec2UniformValueByName(aName);
		}
		vec3							getVec3UniformValueByName(const std::string& aName) {
			return mVDUniform->getVec3UniformValueByName(aName);
		}
		vec4							getVec4UniformValueByName(const std::string& aName) {
			return mVDUniform->getVec4UniformValueByName(aName);
		}
		int								getIntUniformValueByName(const std::string& aName) {
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
		//void							createFloatUniform(const string& aName, int aCtrlIndex, float aValue = 0.01f, float aMin = 0.0f, float aMax = 1.0f);
		//void							createSampler2DUniform(const string& aName, int aCtrlIndex, int aTextureIndex = 0);
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		VDUniformRef					mVDUniform;
		std::map<int, int>					freqIndexes;
		bool							mAudioBuffered;
		ci::gl::TextureRef				mAudioTexture;
		gl::Texture2d::Format			mAudioFormat;
		unsigned char					dTexture[256];// MUST be < mWindowSize

		// shaders
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
		
	};
}
