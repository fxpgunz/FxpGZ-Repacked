/*===========================================================================*\
 |    File: IconMaterial.h
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <Graphics/BaseMaterialHandle.h>
#include <Graphics/TextureHandle.h>

#include "./CGdiBitmap.h"

namespace MaxSDK { namespace Graphics {

	TextureHandle CreateTexture(CGdiBitmap* pBitmap);

	BaseMaterialHandle CreateMaterial(TextureHandle hTexture);

} } // namespaces
