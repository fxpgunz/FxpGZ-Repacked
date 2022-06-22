#ifndef _NHN_USA_POLL
#define _NHN_USA_POLL



class ZNHNUSA_Poll
{
protected:
	bool	bInitialized;



public :
	ZNHNUSA_Poll();

	
	bool ZHanPollInit( const char* pszGameId, const char* pszMemberId, int nServiceCode);

	bool ZHanPollInitGameString( const char* pszGameString);
	
	int ZHanPollProcess( const char* pszData);


	static ZNHNUSA_Poll& GetInstance()
	{
		static ZNHNUSA_Poll m_Poll;
		return m_Poll;
	}
};


inline ZNHNUSA_Poll& GetNHNUSAPoll()
{
	return ZNHNUSA_Poll::GetInstance();	
}


#endif
