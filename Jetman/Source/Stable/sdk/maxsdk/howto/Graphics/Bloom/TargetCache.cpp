/*===========================================================================*\
 |    File: TargetCache.cpp
 |
 | Purpose: Demonstrate post-processing (bloom) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "./TargetCache.h"

namespace MaxSDK { namespace Graphics {

TargetCache::TargetCache()
{
	// do nothing.
}

TargetCache::~TargetCache()
{
	// do nothing.
}

void TargetCache::ValidateTargets(
	size_t originalTargetWidth, 
	size_t originalTargetHeight, 
	TargetFormat format)
{
	bool requireBuild = false;
	if (!mColorTarget_1_2.IsValid())
	{
		requireBuild = true;
	}
	else
	{
		if (mColorTarget_1_2.GetWidth() != originalTargetWidth / 2 || 
			mColorTarget_1_2.GetHeight() != originalTargetHeight / 2)
		{
			requireBuild = true;
		}
	}

	if (!requireBuild)
	{
		return;
	}

	mTempColorTarget_1_2.Initialize(
		GetValidNumber(originalTargetWidth / 2), 
		GetValidNumber(originalTargetHeight / 2), 
		format);
	mColorTarget_1_2.Initialize(
		GetValidNumber(originalTargetWidth / 2), 
		GetValidNumber(originalTargetHeight / 2), 
		format);
	mTempColorTarget_1_4.Initialize(
		GetValidNumber(originalTargetWidth / 4), 
		GetValidNumber(originalTargetHeight / 4), 
		format);
	mColorTarget_1_4.Initialize(
		GetValidNumber(originalTargetWidth / 4), 
		GetValidNumber(originalTargetHeight / 4), 
		format);
	mTempColorTarget_1_8.Initialize(
		GetValidNumber(originalTargetWidth / 8), 
		GetValidNumber(originalTargetHeight / 8), 
		format);
	mColorTarget_1_8.Initialize(
		GetValidNumber(originalTargetWidth / 8), 
		GetValidNumber(originalTargetHeight / 8), 
		format);
}

size_t TargetCache::GetValidNumber(size_t val) const
{
	if (val == 0)
	{
		return 1;
	}

	return val;
}

} } // namespaces
