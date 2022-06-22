// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifdef _DEBUG
	#define _SECURE_SCL 0
	#define _ITERATOR_DEBUG_LEVEL 0
#else
	#define _SECURE_SCL 0
	#define _ITERATOR_DEBUG_LEVEL 0
#endif

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#ifndef _STDAFX_H
#define _STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "targetver.h"

#define POINTER_64 __ptr64
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define NO_WARN_MBCS_MFC_DEPRECATION
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <crtdbg.h>
#include <mbstring.h>
#include <tchar.h>

#include <list>
#include <vector>
#include <map>
#include <string>
#include <list>

#include "d3dx9math.h"

#include "MDebug.h"
#include "MZFileSystem.h"
#include "fileinfo.h"
#include "MXml.h"
#include "RTypes.h"
#include "RMesh.h"
//#include "ThemidaSDK.h"
#define _RFLASH

#endif