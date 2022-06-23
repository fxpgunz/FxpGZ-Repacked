/*===========================================================================*\
 |    File: IconRenderStage.h
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include "./IconWorld.h"
#include <Graphics/ViewSystem/ViewParameter.h>

#include <Graphics/CustomRenderStageHandle.h>

namespace MaxSDK { namespace Graphics {

class IconRenderStage : public MaxHeapOperators
{
public:
	IconRenderStage();
	~IconRenderStage();

	void SetColorTarget(const TargetHandle& pColorTarget);
	void SetDepthTarget(const TargetHandle& pDepthTarget);

	void Render(const ViewParameter& viewParam);

private:
	CameraPtr mpCamera;
	IconWorld mIconWorld;
	CustomRenderStageHandle mCustomRenderStage;
};

} } // namespaces
