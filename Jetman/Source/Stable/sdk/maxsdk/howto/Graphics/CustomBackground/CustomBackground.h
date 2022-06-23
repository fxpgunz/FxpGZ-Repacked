/*===========================================================================*\
 |    File: CustomBackground.h
 |
 | Purpose: Demonstrate custom background in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <Graphics/FragmentGraph/ViewFragment.h>
#include "./CustomBackgroundUtils.h"

namespace MaxSDK { namespace Graphics {

class CustomBackground : public MaxSDK::Graphics::ViewFragment
{
public:
	CustomBackground();
	virtual ~CustomBackground();

	virtual bool DoEvaluate(MaxSDK::Graphics::EvaluationContext* evaluationContext);
	virtual bool OnMessage(const MaxSDK::Graphics::FragmentMessageParameter& messageParameter);
	virtual Class_ID GetClassID() const;

private:
	HWND mHwnd;
	ViewExp* mpViewExp;
	CameraPtr mpCamera;
	DrawQuadWorld mQuadWorld;
	CustomRenderStageHandle mCustomRenderStage;
};

} } // namespaces
