/*===========================================================================*\
 |    File: IconWorld.h
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <Graphics/CustomRenderItemHandle.h>
#include <Graphics/TextureMaterialHandle.h>
#include <Graphics/ViewSystem/ViewParameter.h>

#include <Graphics/RenderWorldHandle.h>

#include <vector>

#include "./CGdiBitmap.h"

namespace MaxSDK { namespace Graphics {

class IconWorld : public MaxHeapOperators
{
public:
	IconWorld();
	~IconWorld();

	bool Initialize();
	RenderWorldHandle GetWorldHandle();

	void UpdateIcons(const ViewParameter& view);

private:
	RenderNodeHandle CreateRenderNode(TextureHandle hTexture);

private:
	bool mInitialized;
	RenderWorldHandle mRenderWorld;
	std::vector<TextureHandle> mTextures;
	std::vector<CGdiBitmap*> mBitmaps;
};

} } // namespaces
