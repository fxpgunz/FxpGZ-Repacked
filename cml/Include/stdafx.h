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

#define _CRT_SECURE_NO_WARNINGS

#ifndef _STDAFX_H
#define _STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define POINTER_64 __ptr64

// stl include
#include <stdio.h>
#include <list>
#include <map>

#include "winsock2.h"
#include "windows.h"
//#include "ThemidaSDK.h"

#endif