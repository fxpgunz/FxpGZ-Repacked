// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#define _SECURE_SCL 0 
#define _ITERATOR_DEBUG_LEVEL 0

#undef _ASSERT
#define _ASSERT(expr) ((void)0)
#undef ASSERT
#define ASSERT(f)          ((void)0)

#define _CRT_SECURE_NO_WARNINGS 1

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		       
#endif

#ifndef WINVER                                   
#define WINVER 0x0600                       
#endif

#ifndef _WIN32_WINNT                     
#define _WIN32_WINNT 0x0600                 
#endif						

#ifndef _WIN32_WINDOWS                   
#define _WIN32_WINDOWS 0x0410             
#endif

#ifndef _WIN32_IE                                 
#define _WIN32_IE 0x0700                    
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	     

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#define POINTER_64 __ptr64
#define NO_WARN_MBCS_MFC_DEPRECATION

#include <afxwin.h>               
#include <afxext.h>           
#include <afxdisp.h>           

#include <afxdtctl.h>		        
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			      
#endif  
#include <afxrich.h>
#include <afxcview.h>

#include <winsock2.h>
#include <windows.h>
#include <comdef.h>
#include <stdio.h>
#include <fstream>

#include "MBMatchServer.h"

//#define _MONITORING

#include "MFeatureDefine.h"
