/*===========================================================================*\
 |    File: Bloom.h
 |
 | Purpose: Demonstrate post-processing (bloom) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <Graphics/FragmentGraph/ViewFragment.h>

#include "./BloomUtils.h"
#include "./TargetCache.h"

namespace MaxSDK { namespace Graphics {

class Bloom : public MaxSDK::Graphics::ViewFragment
{
public:
	Bloom();
	virtual ~Bloom();

	virtual bool DoEvaluate(MaxSDK::Graphics::EvaluationContext* evaluationContext);
	virtual bool OnMessage(const MaxSDK::Graphics::FragmentMessageParameter& messageParameter);
	virtual Class_ID GetClassID() const;

private:
	void SetupRenderStage();
	void RenderPass(
		const TargetHandle& colorBuffer, 
		const TargetHandle& texture, 
		HLSLMaterialHandle material);

private:
	HWND mHwnd;
	ViewExp* mpViewExp;
	TargetCache mTargetCache;
	CameraPtr mpCamera;
	DrawQuadWorld mQuadWorld;
	CustomRenderStageHandle mCustomRenderStage;
};

} } // namespaces
