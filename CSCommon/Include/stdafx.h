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

#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifndef _STDAFX_H
#define _STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define POINTER_64 __ptr64
#define _WIN32_WINNT 0x0501
#include <sdkddkver.h>

#include <afxdb.h>
#include <afxtempl.h>
#include <afxdtctl.h>

#include <Winsock2.h>
#include <mswsock.h>
#include <crtdbg.h>
#include <windows.h>
#include <stdlib.h>

#include <string.h>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
//#include "../sdk/themida/include/ThemidaSDK.h"

#ifndef _PUBLISH

//	#define _CLASSIC					// 클래식 모드용 디파인
#endif

#include "../MFeatureDefine.h"

#define _BLOCK_HACKER	// 해킹관련 정보를 DB에 남기는 작업.


#include "MLocaleDefine.h"
#include "MDebug.h"
#include "MMatchDebug.h"
#include "MXml.h"

#include "MUID.h"
#include "MMatchGlobal.h"
#include "MMatchUtil.h"
#include "MSharedCommandTable.h"
#include "MCommand.h"
#include "MCommandParameter.h"
#include "MCommandCommunicator.h"
#include "MErrorTable.h"
#include "MServer.h"
#include "MMatchServer.h"
#include "MMatchClient.h"
#include "MObject.h"
#include "MMatchItem.h"
#include "MMatchObjCache.h"
#include "MMatchStage.h"
#include "MMatchObject.h"
#include "MMatchChannel.h"


#include "MMatchDebug.h"

#endif