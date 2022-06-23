#ifndef _MBMATCHSERVERCONFIGRELOADER
#define _MBMATCHSERVERCONFIGRELOADER


/*
 ReloadList.xml�� ���� ���� �̸��� �о������ ��� �ҹ��ڷ� ��ȭ�ؼ� ó����. - by SungE

 ���� ���ε� ������ ���. 
	server.ini
	zitem.xml
	hashmap.txt
	event.xml
	eventlist.xml
 */


#define RELOAD_LIST "ReloadList.xml"


#include <vector>
#include <string>
#include <map>
using namespace std;


class MBMatchServerReloadObj
{
public :
	MBMatchServerReloadObj() 
		: m_bIsReloadOK( false )
	{
		memset( &m_LastReloadTime, 0, sizeof(SYSTEMTIME) );
	}

	virtual ~MBMatchServerReloadObj() 
	{
	}

	bool Reload()
	{
		if( m_strFileName.empty() ) return false;
		SetReloadStatus( false );

		const bool bIsReloadOK = OnReload();

		SetReloadStatus( bIsReloadOK );
		if( bIsReloadOK )
			GetLocalTime( &m_LastReloadTime );
		
		return bIsReloadOK;
	}

	const string&		GetFileName() const			{ return m_strFileName; }
	const bool			IsReloadOK() const			{ return m_bIsReloadOK; }
	const SYSTEMTIME&	GetLastReloadTime() const	{ return m_LastReloadTime; }

	void SetFileName( const string& strFileName )	{ m_strFileName = strFileName; }
	
private :
	void SetReloadStatus( const bool bIsReloadOK )	{ m_bIsReloadOK = bIsReloadOK; }

	virtual bool OnReload() = 0;

private :
	string		m_strFileName;
	bool		m_bIsReloadOK;
	SYSTEMTIME	m_LastReloadTime;
};


class MBMatchServerServerIniReloadObj : public MBMatchServerReloadObj
{
private :
	bool OnReload();
};


class MBMatchServerZItemXmlReloadObj : public MBMatchServerReloadObj
{
private :
	bool OnReload();
};


//class MBMatchServerHashmapTxtReloadObj : public MBMatchServerReloadObj
//{
//private :
//	bool OnReload();
//};

class MBMatchServerFileListCrcReloadObj : public MBMatchServerReloadObj
{
private :
	bool OnReload();
};


class MBMatchServerEventXmlReloadObj : public MBMatchServerReloadObj
{
private :
	bool OnReload();
};


class MBMatchServerEventListXmlReloadObj : public MBMatchServerReloadObj
{
private :
	bool OnReload();
};

typedef vector< string >						ReloadFileListVec;
typedef map< string, MBMatchServerReloadObj* >	ReloadObjMap;


class MBMatchServerConfigReloader
{
public :
	MBMatchServerConfigReloader();
	~MBMatchServerConfigReloader();

	bool Reload( const string& strReloadFileList );
	bool Reload( ReloadFileListVec& FileList );
	bool OnReload( const string& strReloadFile );
	bool Create();

	void ClearReloadObjMap();

private :
	bool InitReloadObjMap();
	bool InsertReloadMap( const string& strFileName, MBMatchServerReloadObj* pReloadObj );

private :
	ReloadFileListVec	m_FileList;
	ReloadObjMap		m_ReloadObjMap;
};


#endif