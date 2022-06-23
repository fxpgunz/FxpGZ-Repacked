#ifndef __HanPollForClient__
#define __HanPollForClient__



#define SERVICE_NATION  0x000000FF
#define SERVICE_KOR		0x00000001
#define SERVICE_USA		0x00000002
#define SERVICE_JPN		0x00000003
#define SERVICE_CHN		0x00000004

#define SERVICE_TYPE    0x00000F00
#define SERVICE_ALPHA	0x00000100
#define SERVICE_REAL	0x00000200
#define SERVICE_BETA	0x00000300

#define SERVICE_SITE	0x0000F000
#define SERVICE_HANGAME 0x00001000
#define SERVICE_ASOBLOG 0x00002000

/*
Return value Info:
0 : Ok
- : fault from  function / this system 
-1 : general fault.
+ : fault from poll server
*/

#define HAN_POLL_OK									0
#define HAN_POLL_ARGUMENT_INVALID					-1
#define HAN_POLL_INITED_ALREADY						-2
#define HAN_POLL_INITED_NOT							-3
#define HAN_POLL_INITED_FAIL						-4
#define HAN_POLL_HTTP_INITFAIL						-5
#define HAN_POLL_HTTP_CONNECTFAIL					-6
#define HAN_POLL_HTTP_OPENFAIL						-7
#define HAN_POLL_HTTP_SENDFAIL						-8
#define HAN_POLL_GAMESTRING_SERVICECODEINVALID		-9
#define HAN_POLL_GAMESTRING_GAMEIDINVALID			-10
#define HAN_POLL_GAMESTRING_MEMBERIDINVALID			-11
#define HAN_POLL_POPUPFAIL							-12
#define HAN_POLL_HTTP_RETURNINVALID					-13
#define HAN_POLL_DLL_UNEXPECTED						-100

#define HAN_POLL_ALREADYVOTED						1
#define HAN_POLL_NONEPOLL							2
#define HAN_POLL_USERIDFAILED						3
#define HAN_POLL_GAMEIDFAILED						4
#define HAN_POLL_SVRFAILED							5


#ifdef LOCALE_NHNUSA

#ifdef HANPOLLFORCLIENT_EXPORTS
#define HANPOLL_API __declspec(dllexport)
#else
#define HANPOLL_API __declspec(dllimport)
#endif




// For games that have no gamestring
HANPOLL_API int __stdcall HanPollInit(/*IN*/ char* szGameId,											  
									  /*IN*/ char *szMemberId,
									  /*IN*/ int   nServiceCode);

// For games that have gamestring
HANPOLL_API int __stdcall HanPollInitGameString(/*IN*/ char* szGameString);



HANPOLL_API int __stdcall HanPollProcess(/*IN*/ char* szData);


#else // LOCALE_NHNUSA

int HanPollInit( char* szGameId, char *szMemberId, int nServiceCode)	{ return 0;	}
int HanPollInitGameString( char* szGameString)							{ return 0;	}
int HanPollProcess( char* szData)										{ return 0;	}

#endif

#endif	// __HanPollForClient__