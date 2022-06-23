/*===========================================================================*\
 |    File: Bloom.cpp
 |
 | Purpose: Demonstrate post-processing (bloom) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "Bloom.h"
#include <Graphics/ViewSystem/EvaluationContext.h>

namespace MaxSDK { namespace Graphics {

enum BloomInput
{
	BloomInput_ColorTarget, 
	BloomInput_Count, 
};

enum BloomOutput
{
	BloomOutput_ColorTarget, 
	BloomOutput_Count, 
};

Bloom::Bloom()
{
	mHwnd = NULL;
	mpViewExp = NULL;

	// Construct the inputs of this fragment
	Class_ID inputIDs[BloomInput_Count];
	inputIDs[BloomInput_ColorTarget] = TargetHandle::ClassID();
	InitializeInputs(BloomInput_Count, inputIDs);

	// Construct the outputs of this fragment
	Class_ID outputIDs[BloomOutput_Count];
	outputIDs[BloomOutput_ColorTarget] = TargetHandle::ClassID();
	InitializeOutputs(BloomOutput_Count, outputIDs);

	mCustomRenderStage.Initialize();

	// construct camera
	mpCamera = ICamera::Create();

	mQuadWorld = CreateRenderWorld();
}

Bloom::~Bloom()
{
	// do nothing.
}

/*
Steps of bloom: 

	- filter and downscale original color buffer to small buffer. 
	- downscale small buffer to small buffer 2
	- blur small buffer
	- upscale and add that small buffer to original color buffer.
	- downscale small buffer 2 to small buffer 3
	- blur small buffer 2
	- upscale and add small buffer 2 to original color buffer
	- downscale small buffer 3 to small buffer 4
	- blur small buffer 3
	- upscale and add small buffer 3 to original color buffer

*/
bool Bloom::DoEvaluate(EvaluationContext* evaluationContext)
{
	if (NULL == evaluationContext)
	{
		return false;
	}

	ViewParameter* pViewportParameter = evaluationContext->pViewParameter.GetPointer();

	if (pViewportParameter->IsRenderRegionVisible())
	{
		TargetHandle targetHandle;
		if (!GetSmartHandleInput(targetHandle,BloomInput_ColorTarget) || 
			!targetHandle.IsValid())
		{
			return false;
		}

		const RenderRegion& region = pViewportParameter->GetRenderRegion();
		mTargetCache.ValidateTargets(
			pViewportParameter->GetWidth(), 
			pViewportParameter->GetHeight(), 
			targetHandle.GetFormat());

		SetupRenderStage();

		// Filter and downscale original target to small color target. 
		RenderPass(
			mTargetCache.mColorTarget_1_2, 
			targetHandle, 
			mQuadWorld.hFilterDraw);
		RenderPass(
			mTargetCache.mColorTarget_1_4, 
			mTargetCache.mColorTarget_1_2, 
			mQuadWorld.hDraw);
		RenderPass(
			mTargetCache.mColorTarget_1_8, 
			mTargetCache.mColorTarget_1_4, 
			mQuadWorld.hDraw);

		RenderPass(
			mTargetCache.mTempColorTarget_1_2, 
			mTargetCache.mColorTarget_1_2, 
			mQuadWorld.hGuassianBlurH);
		RenderPass(
			mTargetCache.mColorTarget_1_2, 
			mTargetCache.mTempColorTarget_1_2, 
			mQuadWorld.hGuassianBlurV);
		RenderPass(
			targetHandle, 
			mTargetCache.mColorTarget_1_2, 
			mQuadWorld.hAddDraw);

		RenderPass(
			mTargetCache.mTempColorTarget_1_4, 
			mTargetCache.mColorTarget_1_4, 
			mQuadWorld.hGuassianBlurH);
		RenderPass(
			mTargetCache.mColorTarget_1_4, 
			mTargetCache.mTempColorTarget_1_4, 
			mQuadWorld.hGuassianBlurV);
		RenderPass(
			targetHandle, 
			mTargetCache.mColorTarget_1_4, 
			mQuadWorld.hAddDraw);

		RenderPass(
			mTargetCache.mTempColorTarget_1_8, 
			mTargetCache.mColorTarget_1_8, 
			mQuadWorld.hGuassianBlurH);
		RenderPass(
			mTargetCache.mColorTarget_1_8, 
			mTargetCache.mTempColorTarget_1_8, 
			mQuadWorld.hGuassianBlurV);
		RenderPass(
			targetHandle, 
			mTargetCache.mColorTarget_1_8, 
			mQuadWorld.hAddDraw);

		return SetSmartHandleOutput(targetHandle, BloomOutput_ColorTarget);
	}

	return false;
}

bool Bloom::OnMessage(const MaxSDK::Graphics::FragmentMessageParameter& messageParameter)
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

void Bloom::SetupRenderStage()
{
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
}

void Bloom::RenderPass(
	const TargetHandle& colorBuffer, 
	const TargetHandle& texture, 
	HLSLMaterialHandle material)
{
	Point4 viewPort;
	viewPort.x = 0;
	viewPort.y = 0;
	viewPort.z = colorBuffer.GetWidth();
	viewPort.w = colorBuffer.GetHeight();
	mCustomRenderStage.SetViewport(viewPort);
	mCustomRenderStage.SetColorTarget(colorBuffer);
	{
		material.SetTextureParameter(_T("texturen2"), texture);
		Point3 viewSize(
			colorBuffer.GetWidth(), 
			colorBuffer.GetHeight(), 
			0.0f);
		material.SetFloat3Parameter(_T("amg_viewSize"), viewSize);
		mCustomRenderStage.SetOverrideMaterial(material);
	}
	mCustomRenderStage.Run();
}

} } // namespaces
