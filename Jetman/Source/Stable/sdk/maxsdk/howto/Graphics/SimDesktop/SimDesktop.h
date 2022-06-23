/*===========================================================================*\
 |    File: SimDesktop.h
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <Graphics/FragmentGraph/ViewFragment.h>
#include <Graphics/BaseRasterHandle.h>
#include <Graphics/TargetHandle.h>

#include "./IconRenderStage.h"

namespace MaxSDK { namespace Graphics {

class SimDesktop : public MaxSDK::Graphics::ViewFragment
{
public:
	SimDesktop();
	virtual ~SimDesktop();

	virtual bool DoEvaluate(MaxSDK::Graphics::EvaluationContext* evaluationContext);
	virtual bool OnMessage(const MaxSDK::Graphics::FragmentMessageParameter& messageParameter);
	virtual Class_ID GetClassID() const;

private:
	HWND mHwnd;
	ViewExp* mpViewExp;
	IconRenderStage mRenderer;
};

} } // namespaces
