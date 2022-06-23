#ifndef _MBMATCH_GAMEGUARD
#define _MBMATCH_GAMEGUARD



#include "MBMatchUserSecurityInfo.h"



// Error code 14 : ���Ӱ��� ���� �˻� �����̸� Ŭ���̾�Ʈ�ʿ��� �ֽŹ����� ���Ӱ��带 ������� ������ �߻��մϴ�.



#define GAMEGUARD_DLL_PATH		".\\"
#define ACTIVE_NUM				50
#define GAMEGUARD_CHECKTIME		(1000 * 60 * 3) // 3min
#define MAX_ELAPSEDTIME			(GAMEGUARD_CHECKTIME * 2) // 6min



class MBMatchGGSvrRef;



class MBMatchGameguard : public MBMatchUserSecurityInfo
{
public :
	MBMatchGameguard( const DWORD dwCurTime );
	~MBMatchGameguard();


	const bool Init();
	void Close();

	// server���� client�� ������ AuthData�� �����ϴ� ����
	//   CreateAuthQuery()�� ȣ���ϸ� �������� �����Ͱ� �����ȴ�.
	//   GetAuthQuery()�� ������ �������� �����͸� �����´�.

	// client�κ��� server�� ������ AuthData�˻� ����.
	//  SetAuthAnswer()�� ������� ���� �����͸� �����Ѵ�.
	//  CheckAuthAnswer() ����� �����͸� �˻��Ѵ�.

	const MBMatchGGSvrRef* GetServerAuth() { return m_pServerAuth; }
	void SetAuthAnswer( const DWORD dwIndex, const DWORD dwValue1, const DWORD dwValue2, const DWORD dwValue3 );
		

	bool CreateAuthQuery();
	bool CheckAuthAnswer( const DWORD dwCurTime );


	const DWORD GetLastError()			{ return m_dwLastError; }
	const DWORD GetCreateAuthCount()	{ return m_dwCreateAuthCount; }

private :
	MBMatchGameguard() {}
	
private :
	MBMatchGGSvrRef*	m_pServerAuth;
	bool				m_bIsInit;
	DWORD				m_dwLastError;
	DWORD				m_dwCreateAuthCount;
};


const DWORD InitMBMatchGameguardAuth();
void CleanupMBMatchGameguardAuth();
void UpdateGambeguard( const DWORD dwCurTime );


#endif