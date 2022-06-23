/*===========================================================================*\
 |    File: CResource.cpp
 |
 | Purpose: Utility class for loading project resources. 
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#include "CResource.h"

CResource::CResource()
{
	mhResource = NULL;
	mpResourceData = NULL;
	mResourceSize = 0;
}

CResource::~CResource()
{
	clear();
}

void CResource::clear()
{
	mhResource = NULL;
	mpResourceData = NULL;
	mResourceSize = 0;
}

bool CResource::load(const TCHAR* resName, const TCHAR* resType, HMODULE hModule)
{
	clear();

	mhResource = ::FindResource(hModule, resName, resType);
	if (!mhResource)
	{
		return false;
	}
	mResourceSize = ::SizeofResource(hModule, mhResource);
	if (mResourceSize == 0)
	{
		return false;
	}
	mpResourceData = ::LockResource(::LoadResource(hModule, mhResource));
	if (mpResourceData == NULL)
	{
		return false;
	}

	return true;
}

bool CResource::load(UINT resId, const TCHAR* resType, HMODULE hModule)
{
	return load(MAKEINTRESOURCE(resId), resType, hModule);
}

bool CResource::load(UINT resId, UINT resType, HMODULE hModule)
{
	return load(MAKEINTRESOURCE(resId), MAKEINTRESOURCE(resType), hModule);
}

const void* CResource::getData() const
{
	return mpResourceData;
}

size_t CResource::getSize() const
{
	return mResourceSize;
}
