/*===========================================================================*\
 |    File: GraphicsUtils.h
 |
 | Purpose: Utility functions for easier usage of Nitrous SDK.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "GraphicsUtils.h"
#include "CGdiBitmap.h"
#include "CResource.h"
#include <Graphics/DeviceCaps.h>
#include <Graphics/IDisplayManager.h>

namespace MaxSDK { namespace Graphics {

TextureHandle createTexture(CGdiBitmap* pBitmapRes)
{
	Gdiplus::Bitmap* pBitmap = pBitmapRes->getUnderlyingBitmap();
	TextureHandle hTexture;
	if (!hTexture.Initialize(
		TextureTypeImage2D, 
		pBitmap->GetWidth(), 
		pBitmap->GetHeight(), 
		0, 
		1, 
		1, 
		TargetFormatA8R8G8B8, 
		ResourceUsageNormal))
	{
		return TextureHandle();
	}

	LockedRect lockedRect;
	if (hTexture.WriteOnlyLockRectangle(0, lockedRect, NULL))
	{
		Gdiplus::Rect gdiRect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
		Gdiplus::BitmapData gdiBmpData;

		memset(&gdiBmpData, 0, sizeof(gdiBmpData));
		if (pBitmap->LockBits(
			&gdiRect, 
			Gdiplus::ImageLockModeRead, 
			PixelFormat32bppARGB, 
			&gdiBmpData) == Gdiplus::Ok)
		{
			unsigned char* pTextureBits = lockedRect.pBits;
			unsigned char* pGdiBits = (unsigned char*)(gdiBmpData.Scan0);
			for (size_t y = 0; y < pBitmap->GetHeight(); ++y)
			{
				memcpy(pTextureBits, pGdiBits, pBitmap->GetWidth() * sizeof(DWORD));
				pTextureBits += lockedRect.pitch;
				pGdiBits += gdiBmpData.Stride;
			}

			pBitmap->UnlockBits(&gdiBmpData);
		}

		hTexture.WriteOnlyUnlockRectangle();
	}

	return hTexture;
}

TextureHandle createTexture(const TCHAR* filename)
{
	CGdiBitmap bitmap;
	if (!bitmap.loadFile(filename))
	{
		return TextureHandle();
	}

	return createTexture(&bitmap);
}

TextureHandle createTexture(const TCHAR* resName, const TCHAR* resType, HMODULE hModule)
{
	CGdiBitmap bitmap;
	if (!bitmap.loadResource(resName, resType, hModule))
	{
		return TextureHandle();
	}

	return createTexture(&bitmap);
}

TextureHandle createTexture(UINT resId, const TCHAR* resType, HMODULE hModule)
{
	CGdiBitmap bitmap;
	if (!bitmap.loadResource(resId, resType, hModule))
	{
		return TextureHandle();
	}

	return createTexture(&bitmap);
}

TextureHandle createTexture(UINT resId, UINT resType, HMODULE hModule)
{
	CGdiBitmap bitmap;
	if (!bitmap.loadResource(resId, resType, hModule))
	{
		return TextureHandle();
	}

	return createTexture(&bitmap);
}

HLSLMaterialHandle createMaterial(
	UINT shaderID_dx9, 
	UINT shaderID_dx11, 
	HMODULE hModule)
{
	UINT shaderID = shaderID_dx9;
	MaxSDK::Graphics::DeviceCaps caps;
	GetIDisplayManager()->GetDeviceCaps(caps);
	if (Level5_0 == caps.FeatureLevel)
	{
		shaderID = shaderID_dx11;
	}

	HLSLMaterialHandle hMaterial;
	CResource resource;
	if (resource.load(shaderID, _T("SHADER"), hModule))
	{
		CStr str;
		str.Resize((int)(resource.getSize() + 1));
		if (resource.getSize() > 0)
		{
			char* pStr = str.dataForWrite();
			memcpy(pStr, resource.getData(), resource.getSize());

			pStr[resource.getSize()] = '\0';
		}

		hMaterial.InitializeWithString(str);
	}

	return hMaterial;
}

} }
