#ifndef _NHN_USA_REPORT
#define _NHN_USA_REPORT

#ifdef LOCALE_NHNUSA

#include ".\\nhn\\HanReportForClient.h"
#define MAX_REPORT_LEN 1024

class ZNHNUSA_Report
{
public :
	ZNHNUSA_Report();
	~ZNHNUSA_Report();

	bool InitReport( const char* szUserID, const char* pszGameStr );

	void ReportStartGame();
	void ReportInitGameGuard();
	void ReportInitComplete();
	void ReportCreateGameScreen();
	void ReportDisplayServerList();
	void ReportDisplaySelCharacter();
	void ReportDisplayCreateCharacter();
	void ReportSelectCharacter();
	void ReportJoinChannel();
	void ReportCompleteJoinChannel();
	void ReportJoinRoom();
	void ReportCompleteGameResult();
	void ReportCloseGame();
	void ReportCrashedGame();
	
	static ZNHNUSA_Report& GetInstance()
	{
		static ZNHNUSA_Report Report;
		return Report;
	}

private :
	char m_szUserID[ 20 ];
};


ZNHNUSA_Report& GetNHNUSAReport();


#endif


#endif