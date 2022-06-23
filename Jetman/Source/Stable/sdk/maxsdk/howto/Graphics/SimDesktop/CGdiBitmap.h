/*===========================================================================*\
 |    File: CGdiBitmap.h
 |
 | Purpose: Demonstrate how to display overlay (e.g. UI) in 3ds max viewport.
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <windows.h>
#include <gdiplus.h>

class CGdiBitmap
{
public:
	CGdiBitmap();
	virtual ~CGdiBitmap();

	void clear();

	bool loadFile(const TCHAR* filename);
	bool saveFile(const TCHAR* filename) const;

	bool loadResource(const TCHAR* resName, const TCHAR* resType, HMODULE hModule);
	bool loadResource(UINT resId, const TCHAR* resType, HMODULE hModule);
	bool loadResource(UINT resId, UINT resType, HMODULE hModule);

	Gdiplus::Bitmap* getUnderlyingBitmap() const;

private:
	CGdiBitmap(const CGdiBitmap&);
	CGdiBitmap& operator = (const CGdiBitmap&);

private:
	Gdiplus::Bitmap* mpBitmap;
};
