/*===========================================================================*\
 |    File: IconMaterial.cpp
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "./IconMaterial.h"
#include "./CResource.h"

// Declaration of ghInstance
#include "./SimDesktopMain.h"
// Definition of IDR_FX_ICON_DX*
#include "./resource.h"

#include <Graphics/HLSLMaterialHandle.h>
#include <Graphics/DeviceCaps.h>
#include <Graphics/IDisplayManager.h>
#include <d3d9.h>

namespace MaxSDK { namespace Graphics {

TextureHandle CreateTexture(CGdiBitmap* pBitmapRes)
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

bool loadShader(HLSLMaterialHandle& hMaterial, UINT shaderID)
{
	CResource resource;
	if (resource.load(shaderID, _T("SHADER"), ghInstance))
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

	return true;
}

bool isDX11()
{
	MaxSDK::Graphics::DeviceCaps caps;
	GetIDisplayManager()->GetDeviceCaps(caps);
	if (Level5_0 == caps.FeatureLevel)
	{
		return true;
	}

	return false;
}

BaseMaterialHandle CreateMaterial(TextureHandle hTexture)
{
	HLSLMaterialHandle hMaterial;

	CResource resource;
	if (isDX11())
	{
		loadShader(hMaterial, IDR_FX_ICON_DX11);
	}
	else
	{
		loadShader(hMaterial, IDR_FX_ICON_DX9);
	}

	hMaterial.SetTextureParameter(_T("texturen2"), hTexture);

	return hMaterial;
}

} } // namespaces
