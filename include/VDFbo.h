/*
	VDFbo
	Handles the frame buffer objects to draw into, from shaders, images, video.
*/
// TODO avoid duplicate get(Float/Int/Bool)UniformValueBy(Name/Index) which exist in VDAnimation too
// TODO recycle using object pool

#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Xml.h"
#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "cinder/ImageIo.h"
// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// shader
#include "VDShader.h"
// textures
#include "VDTexture.h"
//!  Uniforms
#include "VDUniform.h"
// video
//#include "ciWMFVideoPlayer.h"

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDFbo instance
	typedef std::shared_ptr<class VDFbo> 			VDFboRef;
	typedef std::vector<VDFboRef>					VDFboList;
	// for profiling
	typedef std::chrono::high_resolution_clock		Clock;

	class VDFbo {
	public:
		VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const JsonTree &json);
		~VDFbo(void);
		static VDFboRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, const JsonTree &json) {
			return std::make_shared<VDFbo>(aVDSettings, aVDAnimation, json);
		}
		//typedef enum { UNKNOWN, IMAGE, SEQUENCE, MOVIE, CAMERA, SHARED, AUDIO, STREAM } TextureType;
		// get live rendered texture
		ci::gl::Texture2dRef					getTexture();
		ci::gl::Texture2dRef					getRenderedTexture() {
			//last frame rendered
			return mRenderedTexture;
		}
		bool									setFragmentString(const string& aFragmentShaderString, const string& aName = "");
		bool									loadFragmentStringFromFile(const string& aFileName);
		bool									isValid() {
			return mValid;
		};
		/*void									flipV() {
			mVDAnimation->setBoolUniformValueByIndex(mVDSettings->IFLIPV, !mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV));
		};
		void									flipH() {
			mVDAnimation->setBoolUniformValueByIndex(mVDSettings->IFLIPH, !mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH));
		};
		bool									isFlipH() { return mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPV); };
		bool									isFlipV() { return mVDAnimation->getBoolUniformValueByIndex(mVDSettings->IFLIPH); };*/
		std::string								getName() { return mName; };
		std::string								getShaderName() { return mShaderName; };
		std::string								getTextureName() { return mTextureList[0]->getTextureName(); };
		ci::gl::Texture2dRef					getInputTexture() { return mTextureList[0]->getTexture(); };
		string									getStatus() { return mTextureList[0]->getStatus(); };
		void									setImageInputTexture(ci::gl::Texture2dRef aTextureRef, const string& aTextureFilename) { 
			mTextureList[0]->setImageInputTexture( aTextureRef, aTextureFilename);
		};
		void									updateThumbFile() {
			isReady = false;
			getTexture();
			if (shaderToLoad) shaderToLoad->getThumbTexture();
		}
		std::vector<ci::gl::GlslProg::Uniform>	getUniforms() { return mUniforms; };
		ci::JsonTree							toJson(bool save, unsigned int aIndex) const;
		// uniforms
		bool									getGlobal() {
			return mGlobal;
		};
		void									toggleGlobal() {
			mGlobal = !mGlobal;
		};
		
		
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Animation
		VDAnimationRef					mVDAnimation;
		// uniforms
		VDUniformRef					mVDUniform;
		//! Input textures
		VDTextureList					mTextureList;
		unsigned int					mInputTextureIndex;
		unsigned int					createInputTexture(const JsonTree &json) {
			unsigned int rtn = 0;
			VDTextureRef texRef = VDTexture::create(mVDSettings, mVDAnimation, json);
			mTextureList.push_back(texRef);
			rtn = mTextureList.size() - 1;
			return rtn;

		}
		// video
		/*ciWMFVideoPlayer				mVideo;
		float							mVideoPos;
		float							mVideoDuration;
		bool							mIsVideoLoaded;
	*/
	//! shader
		gl::GlslProgRef					mShader;
		VDShaderRef						shaderToLoad;
		std::vector<ci::gl::GlslProg::Uniform> mUniforms;
		string							mShaderName = "";
		string							mName = "";
		string							mShaderFileName = "";

		std::string						mShaderFragmentString;
		std::string						mFileNameWithExtension;
		//fs::path						mFragFile;
		bool							mActive;
		int								mMicroSeconds;
		bool							mGlobal = true;

		// include shader lines
		std::string						shaderInclude;
		string							mError;
		bool							mValid = false;
		//! Fbo
		gl::FboRef						mFbo;

		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		bool							isReady;
		ci::gl::Texture2dRef			mRenderedTexture;
		ci::gl::Texture2dRef			getFboTexture();

		
		
	};
}