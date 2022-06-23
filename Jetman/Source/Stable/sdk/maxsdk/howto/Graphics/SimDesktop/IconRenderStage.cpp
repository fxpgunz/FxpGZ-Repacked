/*===========================================================================*\
 |    File: IconRenderStage.cpp
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "./IconRenderStage.h"

namespace MaxSDK { namespace Graphics {

IconRenderStage::IconRenderStage()
{
	mIconWorld.Initialize();
	mCustomRenderStage.Initialize();

	// construct camera
	mpCamera = ICamera::Create();
}

IconRenderStage::~IconRenderStage()
{
	// do nothing.
}

void IconRenderStage::SetColorTarget(const TargetHandle& pColorTarget)
{
	mCustomRenderStage.SetColorTarget(pColorTarget);
}

void IconRenderStage::SetDepthTarget(const TargetHandle& pDepthTarget)
{
	mCustomRenderStage.SetDepthTarget(pDepthTarget);
}

void IconRenderStage::Render(const ViewParameter& viewParam)
{
	mIconWorld.UpdateIcons(viewParam);

	// specify the view matrix and projection matrix
	Matrix44 matView;
	matView.MakeLookAt(
		// eye pos
		Point3(0, 0, -50), 
		// direction
		Point3(0, 0, 1), 
		// up
		Point3(0, 1, 0));
	mpCamera->SetViewMatrix(matView);

	Matrix44 matProj;
	matProj.MakeOrthoProjectionLH(
		viewParam.GetWidth(), 
		viewParam.GetHeight(), 
		-1000, 
		1000);
	mpCamera->SetProjectionMatrix(matProj);

	mCustomRenderStage.SetClearColorBuffer(false);
	mCustomRenderStage.SetClearDepth(false);
	mCustomRenderStage.SetBackgroundColor(AColor(0.2f, 0.2f, 0.5f));
	mCustomRenderStage.SetCamera(mpCamera);
	mCustomRenderStage.SetRenderWorld(mIconWorld.GetWorldHandle());
	Point4 viewPort;
	viewPort.x = 0;
	viewPort.y = 0;
	viewPort.z = viewParam.GetWidth();
	viewPort.w = viewParam.GetHeight();
	mCustomRenderStage.SetViewport(viewPort);
	mCustomRenderStage.SetVisibilityGroup(RenderItemVisible_Shaded);

	mCustomRenderStage.Run();
}

} } // namespaces
