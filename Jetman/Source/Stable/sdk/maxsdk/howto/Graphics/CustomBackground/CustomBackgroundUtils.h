/*===========================================================================*\
 |    File: CustomBackgroundUtils.h
 |
 | Purpose: Demonstrate custom background in 3ds max viewport.
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

	HLSLMaterialHandle CreateMaterial_Draw();

	RenderNodeHandle CreateRenderNode();

	struct DrawQuadWorld
	{
		RenderWorldHandle hWorld;
		HLSLMaterialHandle hDraw;
	};

	DrawQuadWorld CreateRenderWorld();

} } // namespaces
