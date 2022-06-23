/*===========================================================================*\
 |    File: TargetCache.h
 |
 | Purpose: Demonstrate post-processing (bloom) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include "./BloomUtils.h"

namespace MaxSDK { namespace Graphics {

class TargetCache : public MaxHeapOperators
{
public:
	TargetCache();
	~TargetCache();

	void ValidateTargets(
		size_t originalTargetWidth, 
		size_t originalTargetHeight, 
		TargetFormat format);

private:
	size_t GetValidNumber(size_t val) const;

public:
	TargetHandle mTempColorTarget_1_2;
	TargetHandle mTempColorTarget_1_4;
	TargetHandle mTempColorTarget_1_8;
	TargetHandle mColorTarget_1_2;
	TargetHandle mColorTarget_1_4;
	TargetHandle mColorTarget_1_8;
};

} } // namespaces
