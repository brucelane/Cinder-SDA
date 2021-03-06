#pragma once
#include "VDTypes.h"
#include "VDUIParams.h"

namespace videodromm {
	typedef std::shared_ptr<class VDParams> VDParamsRef;
	class VDParams {
	private:
		static VDParamsRef PARAMS;
		VDUIParams		uiParams;
		const float		mAspectRatio{ 0.5625 }; // ratio 4:3 (0.75) 16:9 (0.5625)
	public:
		static VDParamsRef create();
		VDParams() : uiParams{ } {}
		unsigned getFboWidth() const noexcept {
			return uiParams.getFboWidth();
		};
		unsigned getFboHeight() const noexcept {
			return uiParams.getFboHeight();
		};
		unsigned getPreviewFboWidth() const noexcept {
			return uiParams.getPreviewFboWidth();
		};
		unsigned getPreviewFboHeight() const noexcept {
			return uiParams.getPreviewFboHeight();
		};
		unsigned getPreviewWidth() const noexcept {
			return uiParams.getPreviewWidth();
		};
		unsigned getPreviewHeight() const noexcept {
			return uiParams.getPreviewHeight();
		};		
		unsigned getUIMargin() const noexcept {
			return uiParams.getUIMargin();
		};
		unsigned getUIXPosCol1() const noexcept {
			return uiParams.getUIXPosCol1();
		};
		unsigned getUIXPosCol2() const noexcept {
			return uiParams.getUIXPosCol2();
		};
		
		unsigned getUIXPosCol3() const noexcept {
			return uiParams.getUIXPosCol3();
		};
		unsigned getUIYPosRow1() const noexcept {
			return uiParams.getUIYPosRow1();
		};
		unsigned getUIYPosRow2() const noexcept {
			return uiParams.getUIYPosRow2();
		};
		unsigned getUIYPosRow3() const noexcept {
			return uiParams.getUIYPosRow3();
		}; 
		unsigned getUILargeW() const noexcept {
			return uiParams.getUILargeW();
		};
		unsigned getUILargeH() const noexcept {
			return uiParams.getUILargeH();
		};
		unsigned getUILargePreviewW() const noexcept {
			return uiParams.getUILargePreviewW();
		}; 
		unsigned getUILargePreviewH() const noexcept {
			return uiParams.getUILargePreviewH();
		}; 
		
		~VDParams()
		{

		}
	};

}