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
	struct VDUniform
	{
		string							name;
		int								uniformType;
		int								index;
		float							defaultValue;
		//union {
		float							floatValue;
		bool							boolValue;
		int								intValue;
		vec2							vec2Value;
		vec3							vec3Value;
		vec4							vec4Value;
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

		static VDUniformRef			create(VDSettingsRef aVDSettings)
		{
			return shared_ptr<VDUniform>(new VDUniform(aVDSettings));
		}
	}
}
