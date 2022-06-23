/*===========================================================================*\
 |    File: SimDesktop.cpp
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "SimDesktop.h"

#include <Graphics/ViewSystem/EvaluationContext.h>

namespace MaxSDK { namespace Graphics {

enum SimDesktopInput
{
	SimDesktopInput_ColorTarget, 
	SimDesktopInput_DepthTarget, 
	SimDesktopInput_Count, 
};

enum SimDesktopOutput
{
	SimDesktopOutput_ColorTarget, 
	SimDesktopOutput_Count, 
};

SimDesktop::SimDesktop()
{
	mHwnd = NULL;
	mpViewExp = NULL;

	// Construct the inputs of this fragment
	Class_ID inputIDs[SimDesktopInput_Count];
	inputIDs[SimDesktopInput_ColorTarget] = TargetHandle::ClassID();
	inputIDs[SimDesktopInput_DepthTarget] = TargetHandle::ClassID();
	InitializeInputs(SimDesktopInput_Count, inputIDs);

	// Construct the outputs of this fragment
	Class_ID outputIDs[SimDesktopOutput_Count];
	outputIDs[SimDesktopOutput_ColorTarget] = TargetHandle::ClassID();
	InitializeOutputs(SimDesktopOutput_Count, outputIDs);
}

SimDesktop::~SimDesktop()
{
	// do nothing.
}

bool SimDesktop::DoEvaluate(EvaluationContext* evaluationContext)
{
	if (NULL == evaluationContext)
	{
		return false;
	}

	ViewParameter* pViewportParameter = evaluationContext->pViewParameter.GetPointer();

	if (pViewportParameter->IsRenderRegionVisible())
	{
		TargetHandle targetHandle;
		if (!GetSmartHandleInput(targetHandle,SimDesktopInput_ColorTarget) || 
			!targetHandle.IsValid())
		{
			return false;
		}
		TargetHandle depthHandle;
		if (!GetSmartHandleInput(depthHandle,SimDesktopInput_DepthTarget) || 
			!depthHandle.IsValid())
		{
			return false;
		}

		mRenderer.SetColorTarget(targetHandle);
		mRenderer.SetDepthTarget(depthHandle);
		mRenderer.Render(*pViewportParameter);

		return SetSmartHandleOutput(targetHandle, SimDesktopOutput_ColorTarget);
	}

	return false;
}

bool SimDesktop::OnMessage(const MaxSDK::Graphics::FragmentMessageParameter& messageParameter)
{
	switch(messageParameter.messageID)
	{
	case FragmentMessageIDTimer:
		break;
	default:
		break;
	}

	return true;
}

} } // namespaces
