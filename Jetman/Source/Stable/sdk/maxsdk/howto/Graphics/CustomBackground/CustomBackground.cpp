/*===========================================================================*\
 |    File: CustomBackground.cpp
 |
 | Purpose: Demonstrate custom background in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "CustomBackground.h"
#include <Graphics/ViewSystem/EvaluationContext.h>

namespace MaxSDK { namespace Graphics {

enum CustomBackgroundInput
{
	CustomBackgroundInput_ColorTarget, 
	CustomBackgroundInput_Count, 
};

enum CustomBackgroundOutput
{
	CustomBackgroundOutput_ColorTarget, 
	CustomBackgroundOutput_Count, 
};

CustomBackground::CustomBackground()
{
	mHwnd = NULL;
	mpViewExp = NULL;

	// Construct the inputs of this fragment
	Class_ID inputIDs[CustomBackgroundInput_Count];
	inputIDs[CustomBackgroundInput_ColorTarget] = TargetHandle::ClassID();
	InitializeInputs(CustomBackgroundInput_Count, inputIDs);

	// Construct the outputs of this fragment
	Class_ID outputIDs[CustomBackgroundOutput_Count];
	outputIDs[CustomBackgroundOutput_ColorTarget] = TargetHandle::ClassID();
	InitializeOutputs(CustomBackgroundOutput_Count, outputIDs);

	mCustomRenderStage.Initialize();

	// construct camera
	mpCamera = ICamera::Create();

	mQuadWorld = CreateRenderWorld();
}

CustomBackground::~CustomBackground()
{
	// do nothing.
}

bool CustomBackground::DoEvaluate(EvaluationContext* evaluationContext)
{
	if (NULL == evaluationContext)
	{
		return false;
	}

	ViewParameter* pViewportParameter = evaluationContext->pViewParameter.GetPointer();

	if (pViewportParameter->IsRenderRegionVisible())
	{
		TargetHandle targetHandle;
		if (!GetSmartHandleInput(targetHandle,CustomBackgroundInput_ColorTarget) || 
			!targetHandle.IsValid())
		{
			return false;
		}

		const RenderRegion& region = pViewportParameter->GetRenderRegion();

		// specify the view matrix and projection matrix
		Matrix44 matView;
		matView.MakeIdentity();
		mpCamera->SetViewMatrix(matView);

		Matrix44 matProj;
		matProj.MakeIdentity();
		mpCamera->SetProjectionMatrix(matProj);

		mCustomRenderStage.SetClearColorBuffer(false);
		mCustomRenderStage.SetClearDepth(false);
		mCustomRenderStage.SetBackgroundColor(AColor(0.2f, 0.2f, 0.5f));
		mCustomRenderStage.SetCamera(mpCamera);
		mCustomRenderStage.SetRenderWorld(mQuadWorld.hWorld);
		mCustomRenderStage.SetVisibilityGroup(RenderItemVisible_Shaded);

		Point4 viewPort;
		viewPort.x = 0;
		viewPort.y = 0;
		viewPort.z = targetHandle.GetWidth();
		viewPort.w = targetHandle.GetHeight();
		mCustomRenderStage.SetViewport(viewPort);
		mCustomRenderStage.SetColorTarget(targetHandle);

		{
			HLSLMaterialHandle hMaterial = mQuadWorld.hDraw;
			Point3 viewSize(
				targetHandle.GetWidth(), 
				targetHandle.GetHeight(), 
				0.0f);
			hMaterial.SetFloat3Parameter(_T("amg_viewSize"), viewSize);
			mCustomRenderStage.SetOverrideMaterial(hMaterial);
		}

		mCustomRenderStage.Run();

		return SetSmartHandleOutput(targetHandle, CustomBackgroundOutput_ColorTarget);
	}

	return false;
}

bool CustomBackground::OnMessage(const MaxSDK::Graphics::FragmentMessageParameter& messageParameter)
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
