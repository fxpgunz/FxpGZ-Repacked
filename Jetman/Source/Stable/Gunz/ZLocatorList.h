#pragma once

#include <vector>
#include <string>

using std::vector;
using std::string;

class MZFileSystem;

#define LOCATOR_PORT 8900

class ZLocatorList : public map< int, string >
{
public:
	ZLocatorList(void);
	virtual ~ZLocatorList(void);

	void AddIP( const char* szIP);

	void Clear() { m_LocatorIPList.clear(); }
	const int GetSize() const { return static_cast< int >( m_LocatorIPList.size() ); }
	const string& GetIPByPos( const int nPos ) { return m_LocatorIPList[ nPos ]; }

//	bool Init( MZFileSystem* pFileSystem, const char* pszListFile );

	bool ParseLocatorList( rapidxml::xml_node<>* element );

private :
//	bool LoadXML( MZFileSystem* pFileSystem, const char* pszListFile );
	bool ParseLocator( rapidxml::xml_node<>* element );

private :
	vector< string > m_LocatorIPList;
};