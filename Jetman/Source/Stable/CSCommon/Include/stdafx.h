// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef _STDAFX_H
#define _STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define POINTER_64 __ptr64

// Include
#include <afxdb.h>
#include <afxtempl.h>
#include <afxdtctl.h>

#include <Winsock2.h>
#include <mswsock.h>
#include <crtdbg.h>
#include <windows.h>
#include <stdlib.h>

/*	// for debug publish
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0

#undef _ASSERT
#define _ASSERT(expr) ((void)0)
#undef ASSERT
#define ASSERT(f)          ((void)0)
*/

// stl
#include <string.h>
#include <map>
#include <list>
#include <vector>
#include <algorithm>

#ifndef _PUBLISH

#define _CLASSIC
#endif

#include "MFeatureDefine.h"

#define _QUEST_ITEM
#define _MONSTER_BIBLE
#define _BLOCK_HACKER


#include "MLocaleDefine.h"
#include "MDebug.h"
#include "MMatchDebug.h"
#include "MXml.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

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