#ifndef _MBMATCHUSERSECURITYINFO
#define _MBMATCHUSERSECURITYINFO


#include "MUID.h"


#include <utility>
using std::pair;



class MBMatchUserSecurityInfo
{
public :
	MBMatchUserSecurityInfo(); // �⺻�����ڷ� ������ �� ���� �ϱ����� �ٵ�� ������ �ʾҽ��ϴ�. - by sunge
	MBMatchUserSecurityInfo( const DWORD dwCurTime ) 
	{
		m_dwLastCheckTime = dwCurTime;
	}

	virtual ~MBMatchUserSecurityInfo() {}

	const DWORD GetLastCheckTime() { return m_dwLastCheckTime; }
	
	void UpdateLastCheckTime( const DWORD dwLastCheckTime ) { m_dwLastCheckTime = dwLastCheckTime; }

protected :
	DWORD m_dwLastCheckTime;
};


typedef pair< MUID, MBMatchUserSecurityInfo* > UserSecurityInfoMgrPair;



#endif