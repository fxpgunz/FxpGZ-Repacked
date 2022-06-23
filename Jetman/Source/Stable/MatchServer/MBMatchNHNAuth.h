#ifndef _MBMATCH_NHNMODULE
#define _MBMATCH_NHNMODULE





//
//-- ����.
//SERVICE_KOR
//SERVICE_USA
//SERVICE_JPN
//SERVICE_CHN
//
//-- ���� Ÿ��.
//SERVICE_ALPHA
//SERVICE_REAL
//


// MMatchGlobal.h�� �̵�.
//#define NHN_GAMEID			"u_gunz"
//#define NHN_AUTH_LENGTH		4096
//#define NHN_OUTBUFF_LENGTH	1024





class MBMatchNHMModule
{
private :
	MBMatchNHMModule();

	bool InitAuth( const int nServerMode );

public :
	~MBMatchNHMModule();

	bool InitAuthAlpha();
	bool InitAuthReal();

	bool IsAuthUser( const char* pszUserID, const char* pszAuthString );

	void WriteAuthLog( const int nValue );


	static MBMatchNHMModule& GetInstance() 
	{
		static MBMatchNHMModule nhnmodule;

		return nhnmodule;
	}
};


MBMatchNHMModule& GetNHNModule();

#endif