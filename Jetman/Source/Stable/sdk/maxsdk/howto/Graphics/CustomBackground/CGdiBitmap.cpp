/*===========================================================================*\
 |    File: CGdiBitmap.cpp
 |
 | Purpose: Gdiplus Bitmap.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "./CGdiBitmap.h"
#include "./CResource.h"

class CGdiPlusSystem
{
public:
	static CGdiPlusSystem& singleton();

	~CGdiPlusSystem();

private:
	CGdiPlusSystem();

	ULONG_PTR m_gdiplusToken;
};

CGdiPlusSystem& CGdiPlusSystem::singleton()
{
	static CGdiPlusSystem sSingleton;
	return sSingleton;
}

CGdiPlusSystem::CGdiPlusSystem()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}


CGdiPlusSystem::~CGdiPlusSystem()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

CGdiBitmap::CGdiBitmap()
{
	// make sure gdi plus system is initialized.
	CGdiPlusSystem::singleton();

	mpBitmap = NULL;
}

CGdiBitmap::~CGdiBitmap()
{
	clear();
}

void CGdiBitmap::clear()
{
	delete mpBitmap;
	mpBitmap = NULL;
}

bool CGdiBitmap::loadFile(const TCHAR* filename)
{
	clear();

	mpBitmap = Gdiplus::Bitmap::FromFile(filename);
	return mpBitmap != NULL && mpBitmap->GetLastStatus() == Gdiplus::Ok;
}

static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	using namespace Gdiplus;

	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

bool CGdiBitmap::saveFile(const TCHAR* filename) const
{
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	return mpBitmap->Save(filename, &pngClsid, NULL) == Gdiplus::Ok;
}

bool CGdiBitmap::loadResource(const TCHAR* resName, const TCHAR* resType, HMODULE hModule)
{
	clear();

	CResource resource;
	if (!resource.load(resName, resType, hModule))
	{
		return false;
	}

	HGLOBAL hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, resource.getSize());
	if (hBuffer)
	{
		void* pBuffer = ::GlobalLock(hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, resource.getData(), resource.getSize());

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
			{
				mpBitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
				if (mpBitmap)
				{
					if (mpBitmap->GetLastStatus() == Gdiplus::Ok)
					{
						return true;
					}

					delete mpBitmap;
					mpBitmap = NULL;
				}
			}
			::GlobalUnlock(hBuffer);
		}
		::GlobalFree(hBuffer);
		hBuffer = NULL;
	}

	return mpBitmap != NULL && mpBitmap->GetLastStatus() == Gdiplus::Ok;
}

bool CGdiBitmap::loadResource(UINT resId, const TCHAR* resType, HMODULE hModule)
{
	return loadResource(MAKEINTRESOURCE(resId), resType, hModule);
}

bool CGdiBitmap::loadResource(UINT resId, UINT resType, HMODULE hModule)
{
	return loadResource(MAKEINTRESOURCE(resId), MAKEINTRESOURCE(resType), hModule);
}

Gdiplus::Bitmap* CGdiBitmap::getUnderlyingBitmap() const
{
	return mpBitmap;
}
