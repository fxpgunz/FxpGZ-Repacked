/*===========================================================================*\
 |    File: BloomUtils.h
 |
 | Purpose: Demonstrate post-processing (bloom) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <Graphics/BaseRasterHandle.h>
#include <Graphics/TargetHandle.h>
#include <Graphics/TextureHandle.h>
#include <Graphics/HLSLMaterialHandle.h>

#include <Graphics/RenderWorldHandle.h>
#include <Graphics/CustomRenderStageHandle.h>

namespace MaxSDK { namespace Graphics {

	HLSLMaterialHandle CreateMaterial_GuassianBlurH();
	HLSLMaterialHandle CreateMaterial_GuassianBlurV();
	HLSLMaterialHandle CreateMaterial_Draw();
	HLSLMaterialHandle CreateMaterial_AddDraw();
	HLSLMaterialHandle CreateMaterial_FilterDraw();

	RenderNodeHandle CreateRenderNode(BaseMaterialHandle hMaterial);

	struct DrawQuadWorld
	{
		DrawQuadWorld()
		{ }

		RenderWorldHandle hWorld;
		HLSLMaterialHandle hGuassianBlurH;
		HLSLMaterialHandle hGuassianBlurV;
		HLSLMaterialHandle hDraw;
		HLSLMaterialHandle hAddDraw;
		HLSLMaterialHandle hFilterDraw;
	};

	DrawQuadWorld CreateRenderWorld();

} } // namespaces
