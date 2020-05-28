#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
//!  json
#include "cinder/Json.h"
//!  Settings
#include "VDSettings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace videodromm
{
	// stores the pointer to the VDUniform instance
	typedef std::shared_ptr<class VDUniform> VDUniformRef;

	//enum class UniformTypes { FLOAT, SAMPLER2D, VEC2, VEC3, VEC4, INT, BOOL };
	// cinder::gl::GlslProg::Uniform
	struct VDUniformStruct
	{
		string							name;
		int								uniformType;
		int								index;
		float							defaultValue;
		//union {
		float							floatValue;
		//bool							boolValue;
		//int								intValue;
		//vec2							vec2Value;
		//vec3							vec3Value;
		//vec4							vec4Value;
		//};
		float							minValue;
		float							maxValue;
		bool							autotime;
		bool							automatic;
		bool							autobass;
		bool							automid;
		bool							autotreble;
		int								textureIndex;
		//bool							isValid;
	};

	class VDUniform {
	public:
		VDUniform(VDSettingsRef aVDSettings);

		static VDUniformRef				create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDUniform>(new VDUniform(aVDSettings));
		}
		int								getUniformIndexForName(const string& aName) {
			return shaderUniforms[stringToIndex(aName)].index;
		};
		bool							setBoolUniformValueByIndex(unsigned int aIndex, bool aValue) {
			shaderUniforms[aIndex].floatValue = aValue;
			return aValue;
		}
		void							setIntUniformValueByName(const string& aName, int aValue) {
			if (aName == "") {
				CI_LOG_E("empty error");
			}
			else {
				shaderUniforms[stringToIndex(aName)].floatValue = aValue;
			}
		}
		void							setIntUniformValueByIndex(unsigned int aIndex, int aValue) {

			if (mVDSettings->IBEAT == aIndex) {
				if (aValue != mLastBeat) {
					mLastBeat = aValue;
					if (aValue == 0) setIntUniformValueByIndex(mVDSettings->IBAR, getIntUniformValueByIndex(mVDSettings->IBAR) + 1);
				}
			}
			shaderUniforms[aIndex].floatValue = aValue;
		}
		void							setFloatUniformValueByName(const string& aName, float aValue) {
			if (aName == "") {
				CI_LOG_E("empty error");
			}
			else {
				shaderUniforms[stringToIndex(aName)].floatValue = aValue;
			}
		}
		void setVec2UniformValueByName(const string& aName, vec2 aValue) {
			if (aName == "") {
				CI_LOG_E("empty error");
			}
			else {
				shaderUniforms[stringToIndex(aName + "X")].floatValue = aValue.x;
				shaderUniforms[stringToIndex(aName + "Y")].floatValue = aValue.y;
			}
		}
		void setVec2UniformValueByIndex(unsigned int aIndex, vec2 aValue) {
			//shaderUniforms[aIndex].vec2Value = aValue; //IRES 120 IRESX 121 IRESY 122 TODO COLOR RENDERSIZE
			shaderUniforms[aIndex+1].floatValue = aValue.x;
			shaderUniforms[aIndex+2].floatValue = aValue.y;
		}
		void setVec3UniformValueByName(const string& aName, vec3 aValue) {
			if (aName == "") {
				CI_LOG_E("empty error");
			}
			else {
				//shaderUniforms[stringToIndex(aName)].vec3Value = aValue;
				shaderUniforms[stringToIndex(aName + "X")].floatValue = aValue.x;
				shaderUniforms[stringToIndex(aName + "Y")].floatValue = aValue.y;
				shaderUniforms[stringToIndex(aName + "Z")].floatValue = aValue.z;
			}
		}
		void setVec3UniformValueByIndex(unsigned int aIndex, vec3 aValue) {
			//shaderUniforms[aIndex].vec3Value = aValue;
			shaderUniforms[aIndex + 1].floatValue = aValue.x;
			shaderUniforms[aIndex + 2].floatValue = aValue.y;
			shaderUniforms[aIndex + 3].floatValue = aValue.z;
		}
		void setVec4UniformValueByName(const string& aName, vec4 aValue) {
			if (aName == "") {
				CI_LOG_E("empty error");
			}
			else {
				//shaderUniforms[stringToIndex(aName)].vec4Value = aValue;
				shaderUniforms[stringToIndex(aName + "X")].floatValue = aValue.x;
				shaderUniforms[stringToIndex(aName + "Y")].floatValue = aValue.y;
				shaderUniforms[stringToIndex(aName + "Z")].floatValue = aValue.z;
				shaderUniforms[stringToIndex(aName + "W")].floatValue = aValue.w;
			}
		}
		void setVec4UniformValueByIndex(unsigned int aIndex, vec4 aValue) {
			//shaderUniforms[aIndex].vec4Value = aValue;
			shaderUniforms[aIndex + 1].floatValue = aValue.x;
			shaderUniforms[aIndex + 2].floatValue = aValue.y;
			shaderUniforms[aIndex + 3].floatValue = aValue.z;
			shaderUniforms[aIndex + 4].floatValue = aValue.w;
		}
		vec2							getVec2UniformValueByName(const string& aName) {
			return vec2(shaderUniforms[stringToIndex(aName + "X")].floatValue,
				shaderUniforms[stringToIndex(aName + "Y")].floatValue);
			//return shaderUniforms[stringToIndex(aName)].vec2Value;
		}
		vec3							getVec3UniformValueByName(const string& aName) {
			return vec3(shaderUniforms[stringToIndex(aName + "X")].floatValue,
				shaderUniforms[stringToIndex(aName + "Y")].floatValue,
				shaderUniforms[stringToIndex(aName + "Z")].floatValue);
			//return shaderUniforms[stringToIndex(aName)].vec3Value;
			// OUI AU TEL
		}
		vec4							getVec4UniformValueByName(const string& aName) {
			return vec4(shaderUniforms[stringToIndex(aName + "X")].floatValue,
				shaderUniforms[stringToIndex(aName + "Y")].floatValue,
				shaderUniforms[stringToIndex(aName + "Z")].floatValue,
				shaderUniforms[stringToIndex(aName + "W")].floatValue);
			//return shaderUniforms[stringToIndex(aName)].vec4Value;
		}
		int								getIntUniformValueByName(const string& aName) {
			return shaderUniforms[stringToIndex(aName)].floatValue;
		};
		bool							getBoolUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].floatValue;
		}
		float							getMinUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].minValue;
		}
		float							getMaxUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].maxValue;
		}
		bool							getBoolUniformValueByName(const string& aName) {
			return shaderUniforms[stringToIndex(aName)].floatValue;
		}
		float							getMinUniformValueByName(const string& aName) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}

			return shaderUniforms[stringToIndex(aName)].minValue;
		}
		float							getMaxUniformValueByName(const string& aName) {
			if (aName == "") {
				CI_LOG_V("empty error");
			}

			return shaderUniforms[stringToIndex(aName)].maxValue;
		}
		float							getFloatUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].floatValue;
		}
		float							getFloatUniformDefaultValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].defaultValue;
		}
		int								getIntUniformValueByIndex(unsigned int aIndex) {
			return shaderUniforms[aIndex].floatValue;
		}
		int								getSampler2DUniformValueByName(const string& aName) {
			return shaderUniforms[stringToIndex(aName)].textureIndex;
		}
		float							getFloatUniformValueByName(const string& aName) {
			if (aName == "") {
				CI_LOG_V("getFloatUniformValueByName name empty");
				return 1.0f;
			}
			else {
				return shaderUniforms[stringToIndex(aName)].floatValue;
			}
		}
		// public for hydra
		void createFloatUniform(const string& aName, int aCtrlIndex, float aValue = 1.0f, float aMin = 0.0f, float aMax = 1.0f) {
			if (aName != "") {
				shaderUniforms[aCtrlIndex].name = aName;
				shaderUniforms[aCtrlIndex].minValue = aMin;
				shaderUniforms[aCtrlIndex].maxValue = aMax;
				shaderUniforms[aCtrlIndex].defaultValue = aValue;
				shaderUniforms[aCtrlIndex].autotime = false;
				shaderUniforms[aCtrlIndex].automatic = false;
				shaderUniforms[aCtrlIndex].autobass = false;
				shaderUniforms[aCtrlIndex].automid = false;
				shaderUniforms[aCtrlIndex].autotreble = false;
				shaderUniforms[aCtrlIndex].index = aCtrlIndex;
				shaderUniforms[aCtrlIndex].floatValue = aValue;
				shaderUniforms[aCtrlIndex].uniformType = 0;
				//shaderUniforms[aCtrlIndex].isValid = true;
			}
		}
		void createSampler2DUniform(const string& aName, int aCtrlIndex, int aTextureIndex) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].textureIndex = aTextureIndex;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = 1;
			//shaderUniforms[aCtrlIndex].isValid = true;
		}

	private:
		// Settings
		VDSettingsRef					mVDSettings;
		map<int, VDUniformStruct>		shaderUniforms;
		fs::path						mUniformsJson;
		//! read a uniforms json file 
		void							loadUniforms(const ci::DataSourceRef& source);
		int								mLastBeat = 0;
		int stringToIndex(const string& key) {
			int rtn = 0;
			if (key == "iTime") {
				rtn = mVDSettings->ITIME;
			}
			else if (key == "iTimeFactor") {
				rtn = mVDSettings->ITIMEFACTOR;
			} // TODO others
			return rtn;
		}
		void							floatFromJson(const ci::JsonTree& json);
		void							sampler2dFromJson(const ci::JsonTree& json);
		void							vec2FromJson(const ci::JsonTree& json);
		void							vec3FromJson(const ci::JsonTree& json);
		void							vec4FromJson(const ci::JsonTree& json);
		void							intFromJson(const ci::JsonTree& json);
		void							boolFromJson(const ci::JsonTree& json);

	
		void createVec2Uniform(const string& aName, int aCtrlIndex, vec2 aValue = vec2(0.0)) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = 2;
			//shaderUniforms[aCtrlIndex].isValid = true;
			shaderUniforms[aCtrlIndex].floatValue = aValue.x;
			//shaderUniforms[aCtrlIndex].vec2Value = aValue;
		}
		void createVec3Uniform(const string& aName, int aCtrlIndex, vec3 aValue = vec3(0.0)) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = 3;
			//shaderUniforms[aCtrlIndex].isValid = true;
			shaderUniforms[aCtrlIndex].floatValue = aValue.x;
			//shaderUniforms[aCtrlIndex].vec3Value = aValue;
		}
		void createVec4Uniform(const string& aName, int aCtrlIndex, vec4 aValue = vec4(0.0)) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = 4;
			//shaderUniforms[aCtrlIndex].isValid = true;
			shaderUniforms[aCtrlIndex].floatValue = aValue.x;
			//shaderUniforms[aCtrlIndex].vec4Value = aValue;
		}
		void createIntUniform(const string& aName, int aCtrlIndex, int aValue = 1) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].uniformType = 5;
			//shaderUniforms[aCtrlIndex].isValid = true;
			shaderUniforms[aCtrlIndex].floatValue = aValue;
			//shaderUniforms[aCtrlIndex].intValue = aValue;
		}
		void createBoolUniform(const string& aName, int aCtrlIndex, bool aValue = false) {
			shaderUniforms[aCtrlIndex].name = aName;
			shaderUniforms[aCtrlIndex].minValue = 0;
			shaderUniforms[aCtrlIndex].maxValue = 1;
			shaderUniforms[aCtrlIndex].defaultValue = aValue;
			//shaderUniforms[aCtrlIndex].boolValue = aValue;
			shaderUniforms[aCtrlIndex].autotime = false;
			shaderUniforms[aCtrlIndex].automatic = false;
			shaderUniforms[aCtrlIndex].autobass = false;
			shaderUniforms[aCtrlIndex].automid = false;
			shaderUniforms[aCtrlIndex].autotreble = false;
			shaderUniforms[aCtrlIndex].index = aCtrlIndex;
			shaderUniforms[aCtrlIndex].floatValue = aValue;
			shaderUniforms[aCtrlIndex].uniformType = 6;
			//shaderUniforms[aCtrlIndex].isValid = true;
		}

	};
};
