/*===========================================================================*\
 |    File: CResource.h
 |
 | Purpose: Utility class for loading project resources. 
 |
 | History: Qilin Ren, Began 2014/09/01.
\*===========================================================================*/

#pragma once

#include <windows.h>

class CResource
{
public:
	CResource();
	virtual ~CResource();

	bool load(const TCHAR* resName, const TCHAR* resType, HMODULE hModule);
	bool load(UINT resId, const TCHAR* resType, HMODULE hModule);
	bool load(UINT resId, UINT resType, HMODULE hModule);

	void clear();

	const void* getData() const;
	size_t getSize() const;

private:
	CResource(const CResource&);
	CResource& operator = (const CResource&);

private:
	HRSRC mhResource;
	const void* mpResourceData;
	size_t mResourceSize;
};
