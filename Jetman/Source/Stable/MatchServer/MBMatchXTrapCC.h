#ifndef _MBMATCH_XTRATCC
#define _MBMATCH_XTRATCC

#include "MBMatchUserSecurityInfo.h"
#include "./XTrap/Xtrap_S_Interface.h"
#include "MemPool.h"

#define XTRAP_CS4_FILE_COUNT			2		// ���� MapFile �� ���� ����
#define MAX_XTRAP_ELAPSEDTIME			20000	// �ݺ� �ֱ� 20��

// Buffer Size
#define XTRAP_CS4_COMSIZE_PACKDATA					128
#define XTRAP_CS4_BUFSIZE_SESSION					320
//#define XTRAP_CS4_BUFSIZE_SESSIONADDON				80
//#define	XTRAP_CS4_BUFSIZE_MAP						13000

// Timeout Limit
#define XTRAP_CS4_MAXTIMEOUT						(int)600
#define XTRAP_CS4_MINTIMEOUT						(int)100



static unsigned char sXTrapCS_Buf[XTRAP_CS4_FILE_COUNT][XTRAP_CS4_BUFSIZE_MAP];	// MapFile�� ���� ����
bool LoadXTrapFile();
bool ReloadXTrapMapFile();

class MBMatchXTrapCC : public MBMatchUserSecurityInfo, public CMemPool<MBMatchXTrapCC>
{
private:

	// 20080904 X-trap ��������Ʈ http://dev:8181/projects/gunz/ticket/175
	// ����� ����  [1] ���� ���� ������ ����� ���� 45000 Byte���� 13000 Byte�� ���� �Ǿ����ϴ�.
	//				[2] ���� ������ ����� ���� 320 + 80*n Byte���� 320 Byte�� �����Ǿ����ϴ�.
	//unsigned char m_sServerBuf[XTRAP_CS4_BUFSIZE_MAP*XTRAP_CS4_FILE_COUNT]; // �ؿ����� �����
	unsigned char m_sServerBuf[XTRAP_CS4_BUFSIZE_SESSION];
	unsigned char m_sComBuf[XTRAP_CS4_COMSIZE_PACKDATA];
	MUID m_UID;
	DWORD m_dwLastUpdateTime;
	
public:
	MBMatchXTrapCC(MUID uId, const DWORD dwCurTime);
	virtual ~MBMatchXTrapCC();

	DWORD XTrapSessionInit();
	DWORD XTrapGetHashData();
	DWORD XTrapCheckHashData(unsigned char *pComBuf);
	void SetXTrapLastUpdateTime(DWORD curTick)		{ m_dwLastUpdateTime = curTick; }
	DWORD GetXTrapLastUpdateTime()					{ return m_dwLastUpdateTime; }
	unsigned char *GetXTrapComBuf()					{ return m_sComBuf; }
};

#endif