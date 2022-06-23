/*===========================================================================*\
 |    File: GraphicsUtils.h
 |
 | Purpose: Utility functions for easier usage of Nitrous SDK.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <Graphics/HLSLMaterialHandle.h>
#include <Graphics/TextureHandle.h>

class CGdiBitmap;

namespace MaxSDK { namespace Graphics {

	TextureHandle createTexture(CGdiBitmap* pBitmap);
	TextureHandle createTexture(const TCHAR* filename);
	TextureHandle createTexture(const TCHAR* resName, const TCHAR* resType, HMODULE hModule);
	TextureHandle createTexture(UINT resId, const TCHAR* resType, HMODULE hModule);
	TextureHandle createTexture(UINT resId, UINT resType, HMODULE hModule);

	HLSLMaterialHandle createMaterial(
		UINT shaderID_dx9, 
		UINT shaderID_dx11, 
		HMODULE hModule);

} }
